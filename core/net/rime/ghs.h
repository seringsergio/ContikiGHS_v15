#ifndef GHS_H
#define GHS_H

/*------------------------------------------------------------------- */
/*----------- INCLUDES ------------------------------------------------ */
/*------------------------------------------------------------------- */

#include "net/linkaddr.h"

/*------------------------------------------------------------------- */
/*----------- DEFINE ------------------------------------------------ */
/*------------------------------------------------------------------- */

//Banderas (flags)
#define EXIST_LOWEST 0x01
#define LINK_WEIGHT_AGREEMENT 0x02

// This #define defines the maximum amount of neighbors we can remember.
#define MAX_NEIGHBORS 16
#define STOP_BROADCAST (MAX_NEIGHBORS * 1)

// These two defines are used for computing the moving average for the
//   broadcast sequence number gaps.
#define SEQNO_EWMA_UNITY 0x100
#define SEQNO_EWMA_ALPHA 0x040


/*------------------------------------------------------------------- */
/*----------GLOBAL VARIABLES -----------------------------------------*/
/*------------------------------------------------------------------- */

extern uint8_t seqno; // sequence number de los paquetes
extern uint8_t flags;

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

// This structure holds information about neighbors.
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
void copy_data( struct neighbor *dest, struct neighbor *source  );





#endif /* GHS_H */
