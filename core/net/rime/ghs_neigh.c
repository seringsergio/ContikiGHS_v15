/*-------------------------------------------------------------------*/
/*---------------- INCLUDES -----------------------------------------*/
/*-------------------------------------------------------------------*/

#include <stdio.h>
#include "ghs_neigh.h"

/*-------------------------------------------------------------------*/
/*---------------- FUNCIONES ----------------------------------------*/
/*-------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Se copian los datos de un vecino a otro.
*/
void ghs_n_copy_data( struct neighbor *dest, struct neighbor *source  )
{
 linkaddr_copy(&dest->addr,  &source->addr);
 dest->last_rssi     = source->last_rssi;
 dest->last_lqi      = source->last_lqi;
 dest->last_seqno    = source->last_seqno;
 dest->avg_seqno_gap = source->avg_seqno_gap;
}

/* Informa de un timeout
*/
void ghs_n_timedout_ruc(const linkaddr_t *to, uint8_t retransmissions)
{
    printf("runicast message timed out when sending to %d.%d, retransmissions %d\n",
  	 to->u8[0], to->u8[1], retransmissions);
}
/* Recibe un mensaje de runicast
*/
void ghs_n_send_ruc(const linkaddr_t *to, uint8_t retransmissions)
{
    printf("runicast message sent to %d.%d, retransmissions %d\n",
       to->u8[0], to->u8[1], retransmissions);
}
/*---------------------------------------------------------------------------*/
/* Funcion que recibe un mensaje de runicast: Guarda en history_list los vecinos que
* han enviado msg y su seq. Si el avg_seqno_gap del vecino es
*  mayor, entonces reemplazo mi avg_seqno_gap.
*/
void ghs_n_recv_ruc(struct neighbor *n_list_head,
                    struct runicast_message *msg, const linkaddr_t *from,
                    struct memb *history_mem, list_t history_list, uint8_t seqno )
{
    // OPTIONAL: Sender history
    struct history_entry *e = NULL;

    for(e = list_head(history_list); e != NULL; e = e->next) {
      if(linkaddr_cmp(&e->addr, from)) { // Si las dir son iguales entra
        break;
      }
    }
    if(e == NULL) {
      // Create new history entry
      e = memb_alloc(history_mem);
      if(e == NULL) {
        e = list_chop(history_list); /* Remove oldest at full history */
      }
      linkaddr_copy(&e->addr, from);
      e->seq = seqno;
      list_push(history_list, e);
    } else {
      // Detect duplicate callback
      if(e->seq == seqno) {
        printf("runicast message received from %d.%d, seqno %d (DUPLICATE)\n",
  	     from->u8[0], from->u8[1], seqno);
        return;
      }
      // Update existing history entry
      e->seq = seqno;
    }

    printf("runicast message received from %d.%d,with avg_seqno_gap = %d.%02d, seqno %d\n",
  	 from->u8[0], from->u8[1],
       (int)(msg->avg_seqno_gap / SEQNO_EWMA_UNITY),
       (int)(((100UL * msg->avg_seqno_gap) / SEQNO_EWMA_UNITY) % 100),
       seqno);

  /* Evaluo el avg_seqno_gap del mensaje recibido, si es mayor que el mio con respecto a
  * ese vecino, entonces reemplazo el avg_seqno_gap. Esto se hace para tener un acuerdo
  * entre el avg_seqno_gap hacia y desde el vecino.
  */
  imponer_avg_seqno_gap(n_list_head, msg, from);

}
/* Evaluo el avg_seqno_gap del mensaje recibido, si es mayor que el mio con respecto a
* ese vecino, entonces reemplazo el avg_seqno_gap. Esto se hace para tener un acuerdo
* entre el avg_seqno_gap hacia y desde el vecino.
*/
void imponer_avg_seqno_gap(struct neighbor *n_list_head, struct runicast_message *msg, const linkaddr_t *from)
{
    struct neighbor *n_aux;

    for( n_aux = n_list_head ; n_aux != NULL; n_aux = list_item_next(n_aux)) // Recorrer toda la lista
    {
        if(linkaddr_cmp(from, &n_aux->addr)) //Si las direcciones son iguales, entra
        {
            if(msg->avg_seqno_gap > n_aux->avg_seqno_gap ) // Si el vecino ve peor caso
            {
                n_aux->avg_seqno_gap = msg->avg_seqno_gap; // Se asigna el peor caso
            }
            break; // Salgo del for
        }
    }

}



/*---------------------------------------------------------------------------*/
/* Exit_Handler: Cuando el proceso de ponerse de acuerdo en los pesos de
* upward y downward de los links finaliza, entonces ordeno la lista de
* vecinos y la imprimo.
*/
void ghs_n_link_weight_worst_exit_handler(struct neighbor *n_list_head, const linkaddr_t *node_addr)
{

    /* Ordenar la lista de vecinos*/
    sort_neighbor_list(n_list_head);

    /* Show the whole list */
    //char string[] = "REEAD";
    //print_neighbor_list(n_list_head, string, node_addr );
}
/*---------------------------------------------------------------------------*/
/* Cuando se termina el proceso de conocer a los vecinos por broadcast, entonces
*  se imprime la lista
*/
void ghs_n_broadcast_neighbor_discovery_exit_handler(struct neighbor *n_list_head,
                                                     const linkaddr_t *node_addr)
{
    printf("Process exited: Neighbor Discovery via Broadcast\n\r");

    /* Show the whole list */
    //char string[] = "REEEAD";
    //print_neighbor_list(n_list_head, string, node_addr );
}

/*---------------------------------------------------------------------------*/
/* Cada vez que llega un broadcast, se evalua si el vecino ya existe, si no existe
*  se crea y se agrega a la lista de vecinos, para el vecino se actualiza: rssi, lqi y seqno;
*  tambien se calcula el nuevo avg_seqno_gap.
*/
void ghs_n_broadcast_recv(struct neighbor *n_list_head,
                          struct broadcast_message *m, const linkaddr_t *from,
                          uint16_t last_rssi, uint16_t last_lqi,
                          struct memb *neigh_memb, list_t neigh_list)
{
    struct neighbor *n;
    uint8_t seqno_gap;

    /* Check if we already know this neighbor. */
    for(n = n_list_head; n != NULL; n = list_item_next(n))
    {
      /* We break out of the loop if the address of the neighbor matches
         the address of the neighbor from which we received this
         broadcast message. */
      if(linkaddr_cmp(&n->addr, from)) {
        break;
      }
    }

    /* If n is NULL, this neighbor was not found in our list, and we
       allocate a new struct neighbor from the neighbors_memb memory
       pool. */
    if(n == NULL) {
      n = memb_alloc(neigh_memb);

      /* If we could not allocate a new neighbor entry, we give up. We
         could have reused an old neighbor entry, but we do not do this
         for now. */
      if(n == NULL) {
        return;
      }

      /* Initialize the fields. */
      linkaddr_copy(&n->addr, from);
      n->last_seqno = m->seqno - 1;
      n->avg_seqno_gap = SEQNO_EWMA_UNITY;

      /* Place the neighbor on the neighbor list. */
      list_add(neigh_list, n);

    }

    /* We can now fill in the fields in our neighbor entry. */
    n->last_rssi = last_rssi;
    n->last_lqi = last_lqi;

    /* Compute the average sequence number gap we have seen from this neighbor. */
    seqno_gap = m->seqno - n->last_seqno;
    n->avg_seqno_gap = (((uint32_t)seqno_gap * SEQNO_EWMA_UNITY) *
                    SEQNO_EWMA_ALPHA) / SEQNO_EWMA_UNITY +
                    ((uint32_t)n->avg_seqno_gap * (SEQNO_EWMA_UNITY -
                                                   SEQNO_EWMA_ALPHA)) /
      SEQNO_EWMA_UNITY;

    /* Remember last seqno we heard. */
    n->last_seqno = m->seqno;

}
/*---------------------------------------------------------------------------*/
/* Imprime la lista de vecinos
*/
void print_neighbor_list(struct neighbor *n_list_head, char *string, const linkaddr_t *node_addr )
{
    struct neighbor *n_aux;
    /* Show the whole list */
    for(n_aux = n_list_head; n_aux != NULL; n_aux = list_item_next(n_aux)) // Recorrer toda la lista
    {
      printf("%s %d %d %d.%02d \n  ",
       string,
       node_addr->u8[0],
       n_aux->addr.u8[0],
       (int)(n_aux->avg_seqno_gap / SEQNO_EWMA_UNITY),
       (int)(((100UL * n_aux->avg_seqno_gap) / SEQNO_EWMA_UNITY) % 100));
    }

}

/* Ordena la lista de vecinos: Se para en la primera posicion,
*  luego recorre (for 2) toda la lista para ver si existe un vecino con
*  menor peso, si existe, intercambian las posiciones. Se repite
*  el procedimiento desde el segundo vecino de la lista (for 1).
*/
void sort_neighbor_list(struct neighbor *n_list_head)
{
    struct neighbor *n_aux, *first_position, *lowest_node = NULL, temp_node;
    uint32_t lowest_avg_seqno_gap;
    /* Sort Linked List in Ascending Order:
       Encuentro el nodo con menor avg_seqno_gap de la lista,
       lo intercambio con el primer elemento de la lista,
       repito lo mismo comenzando del segundo elemento de la lista*/
    for(n_aux = n_list_head;
        n_aux != NULL; n_aux = list_item_next(n_aux)) // Recorrer toda la lista
    {
       flags &= !EXIST_LOWEST;
       //Recorrer desde la segunda posicion del nodo
       for(first_position = n_aux, lowest_avg_seqno_gap =  first_position->avg_seqno_gap;
        first_position != NULL; first_position = list_item_next(first_position))
       {
           if(first_position->avg_seqno_gap < lowest_avg_seqno_gap)
           {
                 lowest_avg_seqno_gap = first_position->avg_seqno_gap;
                 lowest_node = first_position;
                 flags |= EXIST_LOWEST;
           }
       }

       if(flags & EXIST_LOWEST) // Si existe un nodo menor, reemplazo los datos de los nodos
       {
         ghs_n_copy_data(&temp_node, lowest_node);
         ghs_n_copy_data(lowest_node, n_aux);
         ghs_n_copy_data(n_aux, &temp_node);
       }
    }
}
