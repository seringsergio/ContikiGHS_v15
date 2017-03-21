#ifndef GHS_NEIGH_H
#define GHS_NEIGH_H

/*------------------------------------------------------------------- */
/*----------- INCLUDES ------------------------------------------------ */
/*------------------------------------------------------------------- */
#include "contiki.h"
#include "net/linkaddr.h"
#include "lib/list.h"
#include "lib/memb.h"
#include "sys/node-id.h" //Set node id
#include "ghs_algorithm.h"

/*------------------------------------------------------------------- */
/*----------- TYPEDEF ------------------------------------------------ */
/*------------------------------------------------------------------- */

typedef struct wait s_wait;
/*------------------------------------------------------------------- */
/*----------- DEFINE ------------------------------------------------ */
/*------------------------------------------------------------------- */

//Banderas (flags)
#define EXIST_LOWEST               0x01

// Definicion de constantes
#define MAX_RETRANSMISSIONS 4
#define NUM_HISTORY_ENTRIES MAX_NEIGHBORS //Numero de entradas los 16 vecinos posibles
#define WAIT_NETWORK_STABILIZATION 50

#define MAX_NEIGHBORS 16 // This defines the maximum amount of neighbors we can remember.
#define STOP_BROADCAST (MAX_NEIGHBORS * 1) // Detengo el broadcast cuando seqno sea > STOP_BROADCAST

/* These two defines are used for computing the moving average for the
*   broadcast sequence number gaps.
*/
#define SEQNO_EWMA_UNITY 0x100
#define SEQNO_EWMA_ALPHA 0x040

/*------------------------------------------------------------------- */
/*----------EVENTOS -------- -----------------------------------------*/
/*------------------------------------------------------------------- */

//Comunes a todos los procesos
process_event_t e_wait_stabilization; //WAIT_NETWORK_STABILIZATION
//process_event_t e_infinite_wait; //Nunca se debe postear este evento

//neighbor discovery
process_event_t e_discovery_broadcast;
process_event_t e_weight_worst; //WEIGHT_WORST
process_event_t e_init_master_co_i;

/*------------------------------------------------------------------- */
/*----------GLOBAL VARIABLES -----------------------------------------*/
/*------------------------------------------------------------------- */
extern uint8_t flags;  // Banderas del proceso

/*------------------------------------------------------------------- */
/*--------STRUCTURES---------------------------------------------------*/
/*------------------------------------------------------------------- */

// Estructura para saber si ya recibi ese paquete o no.
// Runicast en proceso n_link_weight_worst_case
struct history_entry {
  struct history_entry *next;
  linkaddr_t addr;
  uint8_t seq;
};


// This is the structure of broadcast messages.
struct broadcast_message {
  uint8_t seqno;
};


// This is the structure of runicast messages.
struct runicast_message {
    uint32_t avg_seqno_gap;
};

struct wait
{
    uint8_t seconds;
    struct process *return_process;
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
void ghs_n_recv_ruc(struct neighbor *n_list_head,
                    struct runicast_message *msg, const linkaddr_t *from,
                    struct memb *history_mem, list_t history_list, uint8_t seqno );
void ghs_n_send_ruc(const linkaddr_t *to, uint8_t retransmissions);
void ghs_n_timedout_ruc(const linkaddr_t *to, uint8_t retransmissions);
void ghs_n_link_weight_worst_exit_handler(struct neighbor *n_list_head, const linkaddr_t *node_addr);
void ghs_n_broadcast_neighbor_discovery_exit_handler(struct neighbor *n_list_head, const linkaddr_t *node_addr);
void ghs_n_broadcast_recv(struct neighbor *n_list_head,
                          struct broadcast_message *m, const linkaddr_t *from,
                          uint16_t last_rssi, uint16_t last_lqi,
                          struct memb *neigh_memb, list_t neigh_list);
void print_neighbor_list(struct neighbor *n_list_head, char *string, const linkaddr_t *node_addr );
void sort_neighbor_list(struct neighbor *n_list_head);
void imponer_avg_seqno_gap(struct neighbor *n_list_head, struct runicast_message *msg, const linkaddr_t *from);

void llenar_wait_struct(s_wait *str_wait, uint8_t seconds, struct process *return_process  );


#endif /* GHS_NEIGH_H */
