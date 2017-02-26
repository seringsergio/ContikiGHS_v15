/*
 * Copyright (c) 2010, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         Demonstrates how to use broadcast and unicast
 * \author
 *         Adam Dunkels <adam@sics.se>
 *
 *         This example shows how to send broadcast and unicast, as
 *         well as how to use the Contiki memory block manager (MEMB)
 *         and the Contiki list library (LIST) to keep track of
 *         neighbors. The program consists of two processes, one that
 *         periodically sends broadcast messages and one that
 *         periodically sends unicast messages to random neighbors. A
 *         list of neighbors is maintained. The list is populated from
 *         the reception of broadcast messages from neighbors. The
 *         neighbor list keeps a simple set of quality metrics for
 *         each neighbor: a moving average of sequence number gaps,
 *         which indicates the number of broadcast packets that have
 *         been lost; a the last RSSI received; and the last LQI
 *         received.
 */

#include "contiki.h"
#include "lib/list.h"
#include "lib/memb.h"
#include "lib/random.h"
#include "net/rime/rime.h"

#include <stdio.h>


#define EXIST_LOWEST 0x01


/*Global variables*/
uint8_t seqno = 0; // sequence number de los paquetes
uint8_t flags = 0;

/* This is the structure of broadcast messages. */
struct broadcast_message {
  uint8_t seqno;
};

/* This structure holds information about neighbors. */
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
/* This #define defines the maximum amount of neighbors we can remember. */
#define MAX_NEIGHBORS 16
#define STOP_BROADCAST (MAX_NEIGHBORS * 1)

/* This MEMB() definition defines a memory pool from which we allocate
   neighbor entries. */
MEMB(neighbors_memb, struct neighbor, MAX_NEIGHBORS);

/* The neighbors_list is a Contiki list that holds the neighbors we
   have seen thus far. */
LIST(neighbors_list);

/* These hold the broadcast and unicast structures, respectively. */
static struct broadcast_conn broadcast;

/* These two defines are used for computing the moving average for the
   broadcast sequence number gaps. */
#define SEQNO_EWMA_UNITY 0x100
#define SEQNO_EWMA_ALPHA 0x040


/*---------------------------------------------------------------------------*/
/* We first declare our two processes. */
PROCESS(broadcast_neighbor_discovery, "Neighbor Discovery via Broadcast");
PROCESS(ghs_control, "GHS Control");
/* The AUTOSTART_PROCESSES() definition specifices what processes to
   start when this module is loaded. We put both our processes
   there. */
AUTOSTART_PROCESSES(&broadcast_neighbor_discovery, &ghs_control);
/*---------------------------------------------------------------------------*/
void copy_data( struct neighbor *dest, struct neighbor *source  )
{
 linkaddr_copy(&dest->addr,  &source->addr);
 dest->last_rssi     = source->last_rssi;
 dest->last_lqi      = source->last_lqi;
 dest->last_seqno    = source->last_seqno;
 dest->avg_seqno_gap = source->avg_seqno_gap;
}
/*---------------------------------------------------------------------------*/
static void
broadcast_neighbor_discovery_exit_handler(void)
{
  struct neighbor *n_aux, *first_position, *lowest_node = NULL, temp_node;
  uint32_t lowest_avg_seqno_gap;

  printf("Process exited: Neighbor Discovery via Broadcast\n\r");
  broadcast_close(&broadcast); // Cierro la conexion de broadcast

  /* Sort Linked List in Ascending Order:
     Encuentro el nodo con menor avg_seqno_gap de la lista,
     lo intercambio con el primer elemento de la lista,
     repito lo mismo comenzando del segundo elemento de la lista*/

  for(n_aux = list_head(neighbors_list);
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
       copy_data(&temp_node, lowest_node);
       copy_data(lowest_node, n_aux);
       copy_data(n_aux, &temp_node);
     }
 }

  /* Show the whole list */
  //printf(" \t addr \t | \t avg_seqno_gap | seqno = %d \n\r  ", seqno);
  for(n_aux = list_head(neighbors_list); n_aux != NULL; n_aux = list_item_next(n_aux)) // Recorrer toda la lista
  {
      
    //printf("READ %d.%d %d.%d %d.%02d \n  ",
    printf("READ %d %d %d.%02d \n  ",
      linkaddr_node_addr.u8[0],
	  n_aux->addr.u8[0],
	  (int)(n_aux->avg_seqno_gap / SEQNO_EWMA_UNITY),
	  (int)(((100UL * n_aux->avg_seqno_gap) / SEQNO_EWMA_UNITY) % 100));
  }

}
/*---------------------------------------------------------------------------*/

/* This function is called whenever a broadcast message is received. */
static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  struct neighbor *n;
  struct broadcast_message *m;
  uint8_t seqno_gap;

  /* The packetbuf_dataptr() returns a pointer to the first data byte
     in the received packet. */
  m = packetbuf_dataptr();

  /* Check if we already know this neighbor. */
  for(n = list_head(neighbors_list); n != NULL; n = list_item_next(n)) {

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
    n = memb_alloc(&neighbors_memb);

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
    list_add(neighbors_list, n);

  }

  /* We can now fill in the fields in our neighbor entry. */
  n->last_rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);
  n->last_lqi = packetbuf_attr(PACKETBUF_ATTR_LINK_QUALITY);

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
/* This is where we define what function to be called when a broadcast
   is received. We pass a pointer to this structure in the
   broadcast_open() call below. */
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(ghs_control, ev, data)
{
  static struct etimer et;
  PROCESS_BEGIN();

  while(1)
  {
    /* Delay 2-4 seconds */
    etimer_set(&et, CLOCK_SECOND * 4 + random_rand() % (CLOCK_SECOND * 4));

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    if (seqno > STOP_BROADCAST)
    {
       process_exit(&broadcast_neighbor_discovery);   //Se cierra el proceso y se llama el PROCESS_EXITHANDLER(funcion)
    }

  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(broadcast_neighbor_discovery, ev, data)
{
  static struct etimer et;
  struct broadcast_message msg;

  PROCESS_EXITHANDLER(broadcast_neighbor_discovery_exit_handler());

  PROCESS_BEGIN();

  broadcast_open(&broadcast, 129, &broadcast_call);

  while(1) {

    /* Send a broadcast every 16 - 32 seconds */
    etimer_set(&et, CLOCK_SECOND * 16 + random_rand() % (CLOCK_SECOND * 16));

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    msg.seqno = seqno;
    packetbuf_copyfrom(&msg, sizeof(struct broadcast_message));
    broadcast_send(&broadcast);
    seqno++;
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
