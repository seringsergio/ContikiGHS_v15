#ifndef GHS_H
#define GHS_H

/*------------------------------------------------------------------- */
/*----------- INCLUDES ------------------------------------------------ */
/*------------------------------------------------------------------- */

#include "net/linkaddr.h"
#include "lib/list.h"
#include "lib/memb.h"

/*------------------------------------------------------------------- */
/*----------- DEFINE ------------------------------------------------ */
/*------------------------------------------------------------------- */

//States = Estados del proceso (Neighbor Discrovery)
#define DISCOVERY_BROADCAST        0x01
#define WAIT_NETWORK_STABILIZATION 0x02
#define WEIGHT_WORST               0x04
#define IDLE                       0x10

//Banderas (flags)
#define EXIST_LOWEST               0x01

// Definicion de constantes
#define MAX_NEIGHBORS 16 // This defines the maximum amount of neighbors we can remember.
#define STOP_BROADCAST (MAX_NEIGHBORS * 1) // Detengo el broadcast cuando seqno sea > STOP_BROADCAST

/* These two defines are used for computing the moving average for the
*   broadcast sequence number gaps.
*/
#define SEQNO_EWMA_UNITY 0x100
#define SEQNO_EWMA_ALPHA 0x040

/*------------------------------------------------------------------- */
/*----------GLOBAL VARIABLES -----------------------------------------*/
/*------------------------------------------------------------------- */

extern uint8_t state;  // Estado del proceso
extern uint8_t flags;  // Banderas del proceso

/*------------------------------------------------------------------- */
/*--------STRUCTURES---------------------------------------------------*/
/*------------------------------------------------------------------- */

// This is the structure of broadcast messages.
struct broadcast_message {
  uint8_t seqno;
};

// This is the structure of unicast messages.
struct unicast_message {
    uint32_t avg_seqno_gap;
};

// This structure holds information about neighbors. Es una linked list.
struct neighbor {
  /* The ->next pointer is needed since we are placing these on a
     Contiki list. */
  struct neighbor *next;

  /* The ->addr field holds the Rime address of the neighbor. */
  linkaddr_t addr;

  /* The ->last_rssi and ->last_lqi fields hold the Received Signal
     Strength Indicator (RSSI) and CC2420 Link Quality Indicator (LQI)
     values that are received for the incoming broadcast packets. */
  uint16_t last_rssi, last_lqi;

  /* Each broadcast packet contains a sequence number (seqno). The
     ->last_seqno field holds the last sequenuce number we saw from
     this neighbor. */
  uint8_t last_seqno;

  /* The ->avg_gap contains the average seqno gap that we have seen
     from this neighbor. */
  uint32_t avg_seqno_gap;

};
/*------------------------------------------------------------------- */
/*-----------FUNCIONES-------------------------------------------------*/
/*------------------------------------------------------------------- */

/* Cada una de las funciones esta documentada en ghs_neigh.c  */
void ghs_n_copy_data( struct neighbor *dest, struct neighbor *source  );
void ghs_n_recv_uc(struct neighbor *list_head, struct unicast_message *msg, const linkaddr_t *from );
void ghs_n_sent_uc(const linkaddr_t *dest, const linkaddr_t *linkaddr_null, int status, int num_tx);
void ghs_n_link_weight_worst_exit_handler(struct neighbor *list_head, const linkaddr_t *node_addr);
void ghs_n_broadcast_neighbor_discovery_exit_handler(struct neighbor *list_head, const linkaddr_t *node_addr);
void ghs_n_broadcast_recv(struct neighbor *list_head,
                          struct broadcast_message *m, const linkaddr_t *from,
                          uint16_t last_rssi, uint16_t last_lqi,
                          struct memb *neigh_memb, list_t neigh_list);
void print_neighbor_list(struct neighbor *list_head, char *string, const linkaddr_t *node_addr );
void sort_neighbor_list(struct neighbor *list_head);


#endif /* GHS_H */
