/*-------------------------------------------------------------------*/
/*---------------- INCLUDES -----------------------------------------*/
/*-------------------------------------------------------------------*/

#include "ghs_test_ar.h"




/*-------------------------------------------------------------------*/
/*---------------- FUNCIONES ----------------------------------------*/
/*-------------------------------------------------------------------*/


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

/* Funcion que recibe los msg de runicast
*/
void ghs_test_ar_recv_ruc(void *msg, const linkaddr_t *from,
                         struct memb *history_mem, list_t history_list, uint8_t seqno,
                         struct process *e_test,
                         struct memb *t_mem, list_t t_list, struct process *evaluar_msg_test,
                         struct memb *a_mem, list_t a_list, struct process *evaluar_msg_accept)

{

    /* OPTIONAL: Sender history */
    struct history_entry *e = NULL;
    for(e = list_head(history_list); e != NULL; e = e->next) {
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

       //Leer el packet buffer attribute: Especificamente el tipo de mensaje
       packetbuf_attr_t msg_type = packetbuf_attr(PACKETBUF_ATTR_PACKET_GHS_TYPE_MSG);

       // Evaluo el tipo de msg que llego
       if(msg_type == TEST)
       {
           test_list *t_list_p;

           t_list_p = memb_alloc(t_mem); //Alocar memoria
           if(t_list_p == NULL)
           {
               printf("ERROR: La lista de msg de test esta llena\n");
           }else
           {
               t_list_p->t_msg = *((test_msg *)msg); //msg le hago cast.Luego cojo todo el msg
               linkaddr_copy(&t_list_p->from, from);
               list_push(t_list, t_list_p); //Add an item to the start of the list.
               process_post(evaluar_msg_test, PROCESS_EVENT_CONTINUE, NULL ) ;
           }

       }else
       if(msg_type == M_ACCEPT)
       {
           accept_list *a_list_p;

           a_list_p = memb_alloc(a_mem); //Alocar memoria
           if(a_list_p == NULL)
           {
               printf("ERROR: La lista de msg de accept esta llena\n");
           }else
           {
               linkaddr_copy(&a_list_p->from, from);
               list_push(a_list, a_list_p); //Add an item to the start of the list.
               process_post(evaluar_msg_accept, PROCESS_EVENT_CONTINUE, NULL);
           }
       }else
       if(msg_type == M_REJECT)
       {
           // Evaluo directamente, sin meter en cola, porque el codigo es pequeño

           printf("llego reject de %d \n", from->u8[0]);
           become_rejected(e_list_head_g, from);

           //Si el edge es rechazado, entonces testeo uno nuevo.
           process_post(e_test , PROCESS_EVENT_CONTINUE, NULL);

       }

 } //END receive runicast
