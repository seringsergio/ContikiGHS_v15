/*-------------------------------------------------------------------*/
/*---------------- INCLUDES -----------------------------------------*/
/*-------------------------------------------------------------------*/

#include <stdio.h>
#include "ghs_report_ChaRoot.h"

/*-------------------------------------------------------------------*/
/*---------------- FUNCIONES ----------------------------------------*/
/*-------------------------------------------------------------------*/


/* Funcion para llenar el msg de change root
*/
void llenar_change_root(change_root_msg *cr_msg, const linkaddr_t *next_hop,
                        const linkaddr_t *final_destination)
{
    linkaddr_copy(&cr_msg->next_hop, next_hop);
    linkaddr_copy(&cr_msg->final_destination, final_destination);

}



/* Funcion que recibe los msg de runicast
*/
void ghs_report_ChaRoot_recv_ruc(void *msg,
                         const linkaddr_t *from,
                         struct memb *history_mem, list_t history_list, uint8_t seqno,
                         struct memb *rp_mem, list_t rp_list, struct process *evaluar_msg_rp,
                         list_t  cr_list, struct memb *cr_mem, struct process *evaluar_msg_cr )
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

       if(msg_type == REPORT)
       {

          report_list *rp_list_p;

          rp_list_p = memb_alloc(rp_mem); //Alocar memoria
          if(rp_list_p == NULL)
          {
              printf("ERROR: La lista de msg de REPORT esta llena\n");
          }else
          {
              rp_list_p->rp_msg = *((report_msg *)msg); //msg le hago cast.Luego cojo todo el msg
              linkaddr_copy(&rp_list_p->from, from);
              list_push(rp_list, rp_list_p); //Add an item to the start of the list.
              process_post(evaluar_msg_rp, PROCESS_EVENT_CONTINUE, NULL);
          }

       }else //end IF REPORT
       if(msg_type == CHANGE_ROOT)
       {

           change_root_list *cr_list_p;

           cr_list_p = memb_alloc(cr_mem); //Alocar memoria
           if(cr_list_p == NULL)
           {
               printf("ERROR: La lista de msg de change_root esta llena\n");
           }else
           {
               cr_list_p->cr_msg = *((change_root_msg *)msg); //msg le hago cast.Luego cojo todo el msg
               linkaddr_copy(&cr_list_p->from, from);
               list_push(cr_list, cr_list_p); //Add an item to the start of the list.
               process_post(evaluar_msg_cr, PROCESS_EVENT_CONTINUE, NULL);
           }
       }
}//End recibir mensajes de unicast
