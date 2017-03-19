/*-------------------------------------------------------------------*/
/*---------------- INCLUDES -----------------------------------------*/
/*-------------------------------------------------------------------*/

#include <stdio.h>
#include "ghs_report_ChaRoot.h"

/*-------------------------------------------------------------------*/
/*---------------- FUNCIONES ----------------------------------------*/
/*-------------------------------------------------------------------*/

/* Funcion que recibe los msg de runicast
*/
void ghs_report_ChaRoot_recv_ruc(void *msg, struct history_entry *h_entry_head, const linkaddr_t *from,
                         struct memb *history_mem, list_t history_list, uint8_t seqno,
                         struct memb *report_memb, list_t report_list, struct process *reports_completos
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

   if(msg_type == REPORT)
   {

       report_msg *rp_msg_d = (report_msg *) msg; //rp = report
       /*report_msg  rp_msg;*/

       printf("LLLego report de %d Neigh=%d Weight=%d.%02d Hj=%d flags=%04X\n",
             from->u8[0],
             rp_msg_d->neighbor_r.u8[0],
             (int)(rp_msg_d->weight_r / SEQNO_EWMA_UNITY),
             (int)(((100UL * rp_msg_d->weight_r) / SEQNO_EWMA_UNITY) % 100),
              nd.num_children,
              nd.flags
              );

       //---------------------------------------------------------------
       //Apenas llega el report lo guardo en una lista de Reports
       //---------------------------------------------------------------

       // OPTIONAL: Sender history
       report_str *rp_str = NULL;
       for(rp_str = list_head(report_list) ; rp_str != NULL; rp_str = rp_str->next)
       {
         if(linkaddr_cmp(&rp_str->neighbor, from)) // Si las dir son iguales entra
         {
           break;
         }
       }
       if(rp_str == NULL)
       {
         // Create new history entry
         rp_str = memb_alloc(report_memb);
         if(rp_str == NULL)
         {
           printf("ERROR: NO PUDE CREAR UNA ENTRADA PARA REPORT \n");
         }else
         {
             linkaddr_copy(&rp_str->neighbor, from);
             rp_str->rp_msg = *rp_msg_d;
             list_push(report_list, rp_str);

         }

       } else //Ya existe un report para ese nodo, pero reemplazo la info
       {
         // Update existing report entry
         printf("Ya existe un report para ese nodo, pero reemplazo la info\n");
         linkaddr_copy(&rp_str->neighbor, from);
         rp_str->rp_msg = *rp_msg_d;

       }

       //evaluo si la lista ya esta completa
       printf("Tamano lista=%d \n", list_length(report_list) );
       process_post(reports_completos, PROCESS_EVENT_CONTINUE, NULL);



   }//end IF REPORT


}//End recibir mensajes de unicast

/* Funcion para llenar el msg de change root
*/
void llenar_change_root(change_root_msg *cr_msg, const linkaddr_t *next_hop,
                        const linkaddr_t *final_destination)
{
    linkaddr_copy(&cr_msg->next_hop, next_hop);
    linkaddr_copy(&cr_msg->final_destination, final_destination);

}
