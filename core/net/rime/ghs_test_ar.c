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
                         struct process *send_message_test_ar, edges *e_list_head_g)
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

       printf("llego accept de %d \n", from->u8[0]);
       become_accepted(e_list_head_g, from);
   }else
   if(msg_type == M_REJECT)
   {
       //accept_msg *a_msg = (accept_msg *) msg;

       printf("llego reject de %d \n", from->u8[0]);
       become_rejected(e_list_head_g, from);
   }


}
void become_rejected(edges *e_list_head_g, const linkaddr_t *from)
{
    static edges *e_aux;
    for(e_aux = e_list_head_g; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
    {
        if(linkaddr_cmp(&e_aux->addr, from)) //Entra si las direcciones son iguales
        {
            e_aux->state = E_REJECTED;
            break;
        }
    }

}

void become_accepted(edges *e_list_head_g, const linkaddr_t *from)
{
    static edges *e_aux;
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
                         struct process *e_pospone_test, struct process *e_test)
{
    //printf("Process Init: master_test_ar \n");

    //Terminar procesos
    process_exit(master_co_i);   //Cierro el proceso anterior para liberar memoria

    //Iniciar procesos nuevos
    process_start(send_message_test_ar, NULL);
    process_start(e_pospone_test, NULL);
    process_start(e_test, NULL);
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
