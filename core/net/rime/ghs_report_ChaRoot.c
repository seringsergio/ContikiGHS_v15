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

/*Determina si el nodo es hoja o no dependiendo del # de hijos
*/
uint8_t es_Hoja()
{
    if( (nd.num_children == 0)  )
    {
        return 1; //Si tengo 0 hijos soy hoja
    }else
    {
        return 0;// EN todos los otros casos no soy hoja
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

/* Indica que la lista ya esta completa en el caso de un CORE_NODE
*  en el cual todos los hijos reportaron menos el OTRO_CORE_NODE
*/
uint8_t lista_casi_completa( list_t rp_list)
{
    report_list *rp_list_p;
    uint8_t no_falta_core_node = 0;
    uint8_t lista_completa = 0;

// SI SOY CORE_NODE
//SI solamente falta 1 hijo
// Y ese hijo faltante es el OTRO_CORE_NODE
//Entonces: La lista esta completa


    if(nd.flags & CORE_NODE) //SI SOY CORE_NODE
    {
        if( list_length(rp_list) == (nd.num_children-1)  )  //ME FALTA SOLAMENTE 1 HIJO
        {
            for(rp_list_p = list_head(rp_list); rp_list_p != NULL; rp_list_p = rp_list_p->next)
            {
                if(linkaddr_cmp(&rp_list_p->from , &nd.otro_core_node)) //Entra si las direcciones son iguales
                {
                    no_falta_core_node = 1;

                }
            }

            if(no_falta_core_node == 1)
            {
                lista_completa = 0; //LISTA incompleta
            }else
            {
                lista_completa = 1; //Lista completa
                printf("completa porque solamente falta el otro core_node\n");
            }
        }
    } //END si soy CORE_NODE

    return lista_completa;
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
