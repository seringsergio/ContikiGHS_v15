/*-------------------------------------------------------------------*/
/*---------------- INCLUDES -----------------------------------------*/
/*-------------------------------------------------------------------*/
#include <stdio.h>
#include "ghs_neigh.h"

/*-------------------------------------------------------------------*/
/*---------------- FUNCIONES ----------------------------------------*/
/*-------------------------------------------------------------------*/
void ghs_n_copy_data( struct neighbor *dest, struct neighbor *source  )
{
 linkaddr_copy(&dest->addr,  &source->addr);
 dest->last_rssi     = source->last_rssi;
 dest->last_lqi      = source->last_lqi;
 dest->last_seqno    = source->last_seqno;
 dest->avg_seqno_gap = source->avg_seqno_gap;
}


void ghs_n_recv_uc(struct neighbor *list_head, struct unicast_message *msg, const linkaddr_t *from )
{
  struct neighbor *n_aux;

  printf("unicast ping received from %d.%d with avg_seqno_gap = %d.%02d \n",
         from->u8[0], from->u8[1],
         (int)(msg->avg_seqno_gap / SEQNO_EWMA_UNITY),
         (int)(((100UL * msg->avg_seqno_gap) / SEQNO_EWMA_UNITY) % 100));

  //Escoger el peor caso del avg_seqno_gap (Mio con respecto a vecino)
  for( n_aux = list_head ; n_aux != NULL; n_aux = list_item_next(n_aux)) // Recorrer toda la lista
  {
      if(linkaddr_cmp(from, &n_aux->addr)) //Si las direcciones son iguales
      {
          if(msg->avg_seqno_gap > n_aux->avg_seqno_gap ) // Si el vecino ve peor caso
          {
              n_aux->avg_seqno_gap = msg->avg_seqno_gap; // Se asigna el peor caso
          }
          break; // Salgo del for
      }
  }

}

void ghs_n_sent_uc(const linkaddr_t *dest, const linkaddr_t *linkaddr_null, int status, int num_tx)
{
  if(linkaddr_cmp(dest, linkaddr_null)) {
    return;
  }
  printf("unicast message sent to %d.%d: status %d num_tx %d\n",
    dest->u8[0], dest->u8[1], status, num_tx);
}


void ghs_n_link_weight_worst_exit_handler(struct neighbor *list_head, const linkaddr_t *node_addr)
{
    struct neighbor *n_aux;

    /* Show the whole list */
    for( n_aux = list_head ; n_aux != NULL; n_aux = list_item_next(n_aux)) // Recorrer toda la lista
    {
      printf("READ %d %d %d.%02d \n  ",
        node_addr->u8[0],
        n_aux->addr.u8[0],
        (int)(n_aux->avg_seqno_gap / SEQNO_EWMA_UNITY),
        (int)(((100UL * n_aux->avg_seqno_gap) / SEQNO_EWMA_UNITY) % 100));
    }
}

void ghs_n_broadcast_neighbor_discovery_exit_handler(struct neighbor *list_head, const linkaddr_t *node_addr)
{
    struct neighbor *n_aux, *first_position, *lowest_node = NULL, temp_node;
    uint32_t lowest_avg_seqno_gap;
    printf("Process exited: Neighbor Discovery via Broadcast\n\r");

    /* Sort Linked List in Ascending Order:
       Encuentro el nodo con menor avg_seqno_gap de la lista,
       lo intercambio con el primer elemento de la lista,
       repito lo mismo comenzando del segundo elemento de la lista*/
    for(n_aux = list_head;
        n_aux != NULL; n_aux = list_item_next(n_aux)) // Recorrer toda la lista
    {
       flags &= !EXIST_LOWEST;
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

   /* Show the whole list */
   for(n_aux = list_head; n_aux != NULL; n_aux = list_item_next(n_aux)) // Recorrer toda la lista
   {
     printf("REEAD %d %d %d.%02d \n  ",
      node_addr->u8[0],
 	  n_aux->addr.u8[0],
 	  (int)(n_aux->avg_seqno_gap / SEQNO_EWMA_UNITY),
 	  (int)(((100UL * n_aux->avg_seqno_gap) / SEQNO_EWMA_UNITY) % 100));
   }
}

void ghs_n_broadcast_recv(struct neighbor *list_head,
                          struct broadcast_message *m, const linkaddr_t *from,
                          uint16_t last_rssi, uint16_t last_lqi,
                          struct memb *neigh_memb, list_t neigh_list)
{
    struct neighbor *n;
    //struct broadcast_message *m;
    uint8_t seqno_gap;

    /* The packetbuf_dataptr() returns a pointer to the first data byte
       in the received packet. */
    //m = packetbuf_dataptr();

    /* Check if we already know this neighbor. */
    for(n = list_head; n != NULL; n = list_item_next(n))
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
