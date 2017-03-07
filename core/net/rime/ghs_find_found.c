/*-------------------------------------------------------------------*/
/*---------------- INCLUDES -----------------------------------------*/
/*-------------------------------------------------------------------*/

#include "ghs_find_found.h"

/*-------------------------------------------------------------------*/
/*---------------- FUNCIONES-----------------------------------------*/
/*-------------------------------------------------------------------*/

/* Toma la informacion de la lista de vecinos (neighbors_list) del master_neighbor_discovery
*  y copia la informacion de interes en una nueva lista de edges edges_list.
*/
void fill_edges_list(list_t edges_list, struct memb *edges_memb, struct neighbor *n_list_head)
{
    struct neighbor *n_aux;
    edges *e;
    for(n_aux = n_list_head; n_aux != NULL; n_aux = list_item_next(n_aux)) // Recorrer toda la lista
    {
        e = memb_alloc(edges_memb);        // we allocate a new struct edges from the edges_memb memory pool.

        e -> state  = BASIC; //Todos los edges inician con estado BASIC
        linkaddr_copy(&e->addr,  &n_aux->addr);
        e -> weight = n_aux -> avg_seqno_gap;

        list_add(edges_list, e); //Agregarlo a la lista

    }

}

/* Imprime la lista de edges
*/
void print_edges_list(edges *e_list_head, char *string,  const linkaddr_t *node_addr)
{
    edges *e_aux;

    for(e_aux = e_list_head; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
    {
        printf("%s %d %d %d.%02d %d\n",
              string,
              node_addr->u8[0],
              e_aux->addr.u8[0],
              (int)(e_aux->weight / SEQNO_EWMA_UNITY),
              (int)(((100UL * e_aux->weight) / SEQNO_EWMA_UNITY) % 100),
              e_aux->state);
    }


}

/* Un edge pasa de estado BASIC a BRANCH.
*  Become_branch = Vuelve branch un edge
*/
void become_branch(edges *e_list_head, linkaddr_t *node_addr)
{
    edges *e_aux;

    for(e_aux = e_list_head; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
    {
        if(linkaddr_cmp(&e_aux->addr, node_addr)) //Entra si las direcciones son iguales
        {
            e_aux->state = BRANCH;
            break;
        }
    }
}

/* Devuelve un apuntador al basic edge (su addr) que tenga menor peso.
* Least_basic_edge = Encuentra el basic edge de menor peso.
* (Lista ya ordenada en master_neighbor_discovery)
*/
linkaddr_t* least_basic_edge(edges *e_list_head)
{
    edges *e_aux;

    for(e_aux = e_list_head; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
    {
        if(e_aux->state == BASIC)//tener en cuenta que la lista de edges ya esta
                                  //ordenada de menor a mayor desde el proceso
                                  //master_neighbor_discovery. Entonces solo necesito
                                  //evaluar que el edge sea basic
        {
            break;
        }
    }

    return &e_aux->addr;
}


void ghs_ff_timedout_ruc(const linkaddr_t *to, uint8_t retransmissions)
{
    printf("runicast message timed out when sending to %d.%d, retransmissions %d\n",
  	 to->u8[0], to->u8[1], retransmissions);
}
/* Recibe un mensaje de runicast
*/
void ghs_ff_send_ruc(const linkaddr_t *to, uint8_t retransmissions)
{
    printf("runicast message sent to %d.%d, retransmissions %d\n",
       to->u8[0], to->u8[1], retransmissions);
}
/*---------------------------------------------------------------------------*/
/* Funcion que recibe un mensaje de runicast: Guarda en history_list los vecinos que
* han enviado msg y su seq. Si el avg_seqno_gap del vecino es
*  mayor, entonces reemplazo mi avg_seqno_gap.
*/
void ghs_ff_recv_ruc(struct runicast_message *msg, const linkaddr_t *from,
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

    printf("runicast message received from %d.%d, seqno %d\n",
  	 from->u8[0], from->u8[1],
       seqno);

}
