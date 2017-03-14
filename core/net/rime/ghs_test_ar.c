/*-------------------------------------------------------------------*/
/*---------------- INCLUDES -----------------------------------------*/
/*-------------------------------------------------------------------*/

#include <stdio.h>
#include "ghs_test_ar.h"




/*-------------------------------------------------------------------*/
/*---------------- FUNCIONES ----------------------------------------*/
/*-------------------------------------------------------------------*/

/* Funcion que recibe los msg de runicast
*/
void ghs_test_ar_recv_ruc(void *msg, struct history_entry *h_entry_head, const linkaddr_t *from,
                         struct memb *history_mem, list_t history_list, uint8_t seqno,
                         struct process *send_message_test_ar, edges *e_list_head_g,
                         list_t pt_list, struct memb *pt_memb, struct process *master_test_ar,
                         struct process *e_test, struct process *send_message_report_ChaRoot
                         )
{

    /* OPTIONAL: Sender history */
    struct history_entry *e = NULL;
    for(e = h_entry_head; e != NULL; e = e->next) {
      if(linkaddr_cmp(&e->addr, from)) { // Si las dir son iguales entra
        break;
      }
    }
    if(e == NULL) {
      /* Create new history entry */
      e = memb_alloc(history_mem);
      if(e == NULL) {
        e = list_chop(history_list); /* Remove oldest at full history */
      }
      linkaddr_copy(&e->addr, from);
      e->seq = seqno;
      list_push(history_list, e);
    } else {
      /* Detect duplicate callback */
      if(e->seq == seqno) {
        printf("runicast message received from %d.%d, seqno %d (DUPLICATE)\n",
  	     from->u8[0], from->u8[1], seqno);
        return;
      }
      /* Update existing history entry */
      e->seq = seqno;
    }

    /*printf("runicast TEEEST message received from %d.%d, seqno %d\n",
  	 from->u8[0], from->u8[1], seqno);*/

   //Leer el packet buffer attribute: Especificamente el tipo de mensaje
   packetbuf_attr_t msg_type = packetbuf_attr(PACKETBUF_ATTR_PACKET_GHS_TYPE_MSG);

   // Evaluo el tipo de msg que llego
   if(msg_type == TEST)
   {
      test_msg *t_msg = (test_msg *) msg;
      accept_msg a_msg;
      reject_msg r_msg;

       printf("Reeeeeecibi un TEST msg from %d con name=%d.%02d, level=%d \n",
       from->u8[0],
       (int)(t_msg->f.name / SEQNO_EWMA_UNITY),
       (int)(((100UL * t_msg->f.name) / SEQNO_EWMA_UNITY) % 100),
       t_msg->f.level);

       if(t_msg->f.level > nd.f.level)
       {
           //Pospones processing the incomming test msg, until (t_msg->f.level < nd.f.level)
           pospone_test *pt_aux = NULL;
           for(pt_aux = list_head(pt_list); pt_aux != NULL; pt_aux = list_item_next(pt_aux)) // Recorrer toda la lista
           {
               if(linkaddr_cmp(&pt_aux->neighbor, from)) { // Si las dir son iguales entra
                 break;
               }
           }
           if(pt_aux == NULL) //SI no existe un pospone para el nodo
           {
               // Create new history entry
               pt_aux = memb_alloc(pt_memb);
               if(pt_aux == NULL)
               {
                   printf("ERROR: NO PUDE CREAR UNA ENTRADA PARA POSPONE TEST \n");
               }else
               {

                   llenar_pospone_test(pt_aux, from, *t_msg);
                   list_push(pt_list, pt_aux); //Add an item to the start of the list.
                   printf("Agregado TEST POSPONE de  %d \n", pt_aux->neighbor.u8[0]);
               }
           }else
           {
               printf("Llegaron 2 mensajes de TEST POSPONE del nodo %d \n"
                     ,from->u8[0]);
               //Reemplazo (update) los valores del mensaje de connect
               llenar_pospone_test(pt_aux, from, *t_msg);
           }

       }else
       if(t_msg->f.level <= nd.f.level)
       {
           if(t_msg->f.name == nd.f.name)
           {
               //Enviar reject
               llenar_reject_msg (&r_msg, from);
               process_post(send_message_test_ar, e_msg_reject, &r_msg);
               printf("Quuuiero enviar e_msg_reject a %d \n", r_msg.destination.u8[0]);

           }else
           {
               //Enviar accept
               llenar_accept_msg (&a_msg, from);
               process_post(send_message_test_ar, e_msg_accept, &a_msg);
               printf("Quuuiero enviar e_msg_accept a %d \n", a_msg.destination.u8[0]);
           }
       }
   }else
   if(msg_type == M_ACCEPT)
   {
       //accept_msg *a_msg = (accept_msg *) msg;

       report_msg rp_msg; //rp = report

       printf("llego accept de %d. Numero Hijos = %d \n ", from->u8[0], nd.num_children);
       become_accepted(e_list_head_g, from);

       //Si un edges es aceptado: Se guarda el edge como mejor opcion del Nodo
       linkaddr_copy(&nd.lwoe.node.neighbor,  from);
       nd.lwoe.node.weight = return_weight(e_list_head_g, from);

       //process_post(master_test_ar , e_msg_accept, NULL); //Indico que un edge fue ACEPTADO

       //Ya encontre el ND_LWOE, ahora verifico si tengo hijos o no
       if(nd.num_children == 0) // Si no tengo hijos reporto de una!!
       {
           llenar_report_msg(&rp_msg, &nd.parent , &nd.lwoe.node.neighbor, nd.lwoe.node.weight );
           process_post(send_message_report_ChaRoot, e_msg_report , &rp_msg);
           printf("Deeeseo Reportar Neigh=%d Weight=%d.%02d\n",
                    nd.lwoe.node.neighbor.u8[0],
                    (int)(nd.lwoe.node.weight / SEQNO_EWMA_UNITY),
                    (int)(((100UL * nd.lwoe.node.weight) / SEQNO_EWMA_UNITY) % 100)
                  );


           /*//Si no tengo hijos mi reporte es mi mejor edge
           process_post(send_message_test_ar, e_msg_report , &rp_msg);
           printf("Reporto porque hijos = %d \n", nd.num_children);*/

       }else
       {

       }
       /*process_post(master_test_ar , e_nd_lwoe, NULL); //Llamo el evento para indicar que el nodo ya encontro su best edge
       */

   }else
   if(msg_type == M_REJECT)
   {
       //accept_msg *a_msg = (accept_msg *) msg;

       printf("llego reject de %d \n", from->u8[0]);
       become_rejected(e_list_head_g, from);

       //Si el edge es rechazado, entonces testeo uno nuevo.
       process_post(e_test , PROCESS_EVENT_CONTINUE, NULL);

   }
   else
   if(msg_type == REPORT)
   {
       /*report_msg *rp_msg_d = (report_msg *) msg; //rp = report
       report_msg  rp_msg;*/

       if(nd.flags & CORE_NODE)// Si soy core node
       {
           //Evaluo el
       }
       else
       {
           //Guardo report en una lista

           /*printf("Re-envio report de %d \n", from->u8[0]);
           //Cuando llega un report lo re-envio a mi padre.
           llenar_report_msg(&rp_msg, &nd.parent , &rp_msg_d->neighbor_r, rp_msg_d->weight_r );
           process_post(send_message_test_ar, e_msg_report , &rp_msg);*/

       }

   }


}

/* LLenar el msg de report
*/
void llenar_report_msg(report_msg *rp_msg, const linkaddr_t *destination,
                      const linkaddr_t *neighbor_r, uint32_t weight_r)
{
    linkaddr_copy(&rp_msg->destination, destination);
    linkaddr_copy(&rp_msg->neighbor_r, neighbor_r);
    rp_msg->weight_r  = weight_r;
}


/* Retorna el peso de un edge
*/
uint32_t return_weight(edges *e_list_head_g,  const linkaddr_t *from)
{
    edges *e_aux = NULL;
    for(e_aux = e_list_head_g; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
    {
        if(linkaddr_cmp(&e_aux->addr, from)) //Entra si las direcciones son iguales
        {
            break;
        }
    }
    return (e_aux->weight);
}

/* LLena la estructura de pospone test
*/
void llenar_pospone_test(pospone_test *pt, const linkaddr_t *neighbor, test_msg t_msg)
{
    linkaddr_copy(&pt->neighbor, neighbor);
    pt->t_msg = t_msg;
}
/* Hace que el edge se vuelva rejected
*/
void become_rejected(edges *e_list_head_g, const linkaddr_t *from)
{
    edges *e_aux;
    for(e_aux = e_list_head_g; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
    {
        if(linkaddr_cmp(&e_aux->addr, from)) //Entra si las direcciones son iguales
        {
            e_aux->state = E_REJECTED;
            break;
        }
    }

}
/* Hace que el edges se vuelva accepted
*/
void become_accepted(edges *e_list_head_g, const linkaddr_t *from)
{
    edges *e_aux;
    for(e_aux = e_list_head_g; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
    {
        if(linkaddr_cmp(&e_aux->addr, from)) //Entra si las direcciones son iguales
        {
            e_aux->state = E_ACCEPTED;
            break;
        }
    }
}
/* Funcion para inicializar el proceso master_test_ar
*/
void init_master_test_ar(struct process *master_co_i, struct process *send_message_test_ar,
                         struct process *e_pospone_test, struct process *e_test,
                         struct process *send_message_report_ChaRoot,
                         struct process *reports_completos)
{
    //printf("Process Init: master_test_ar \n");

    //Terminar procesos
    process_exit(master_co_i);   //Cierro el proceso anterior para liberar memoria

    //Iniciar procesos nuevos
    process_start(send_message_test_ar, NULL);
    process_start(e_pospone_test, NULL);
    process_start(e_test, NULL);

    //procesos de ghs_report_ChaRoot
    process_start(send_message_report_ChaRoot, NULL);
    process_start(reports_completos, NULL);
}

/* Funcion para llenar el msg test
*/
void llenar_test_msg (test_msg *t_msg, const linkaddr_t *destination, fragment f)
{
    linkaddr_copy(&t_msg->destination,  destination);
    t_msg->f.name      = f.name;
    t_msg->f.level     = f.level;
}


/* Funcion para llenar el msg de accept
*/
void llenar_accept_msg (accept_msg *a_msg, const linkaddr_t *destination)
{
    linkaddr_copy(&a_msg->destination,  destination);
}

/* Funcion para llenar el msg de reject
*/
void llenar_reject_msg (reject_msg *r_msg, const linkaddr_t *destination)
{
    linkaddr_copy(&r_msg->destination,  destination);
}
