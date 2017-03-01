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

 /*------------------------------------------------------------------- */
 /*----------- INCLUDES ------------------------------------------------ */
 /*------------------------------------------------------------------- */

#include "contiki.h"
#include "lib/list.h"
#include "lib/memb.h"
#include "lib/random.h"
#include "net/rime/rime.h" //Aca esta ghs.h
#include <stdio.h>


/*------------------------------------------------------------------- */
/*----------GLOBAL VARIABLES -----------------------------------------*/
/*------------------------------------------------------------------- */
uint8_t seqno = 0; // sequence number de los paquetes
uint8_t flags = 0;

/* This MEMB() definition defines a memory pool from which we allocate
   neighbor entries. */
MEMB(neighbors_memb, struct neighbor, MAX_NEIGHBORS);

/* The neighbors_list is a Contiki list that holds the neighbors we
   have seen thus far. */
LIST(neighbors_list);

/*------------------------------------------------------------------- */
/*----------STATIC VARIABLES -----------------------------------------*/
/*------------------------------------------------------------------- */
static struct broadcast_conn n_broadcast; // These hold the broadcast structure.
static struct unicast_conn n_uc; // These hold the unicast structure.

/*------------------------------------------------------------------- */
/*----------PROCESSES------- -----------------------------------------*/
/*------------------------------------------------------------------- */

PROCESS(broadcast_neighbor_discovery, "Neighbor Discovery via Broadcast");
PROCESS(link_weight_worst_case, "Assume Worst Weight for Link");
PROCESS(ghs_control, "GHS Control");

AUTOSTART_PROCESSES(&broadcast_neighbor_discovery, &ghs_control, &link_weight_worst_case);

/*---------------------------------------------------------------------------*/
static void n_recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{
  ghs_n_recv_uc(list_head(neighbors_list), packetbuf_dataptr(), from );
}
/*---------------------------------------------------------------------------*/
static void n_sent_uc(struct unicast_conn *c, int status, int num_tx)
{
  ghs_n_sent_uc(packetbuf_addr(PACKETBUF_ADDR_RECEIVER), &linkaddr_null, status, num_tx );
}
static const struct unicast_callbacks unicast_callbacks = {n_recv_uc, n_sent_uc};
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
static void n_link_weight_worst_exit_handler(void)
{
    unicast_close(&n_uc); // Cierro la conexion unicast
    ghs_n_link_weight_worst_exit_handler(list_head(neighbors_list), &linkaddr_node_addr);
}
/*---------------------------------------------------------------------------*/
static void n_broadcast_neighbor_discovery_exit_handler(void)
{
  broadcast_close(&n_broadcast); // Cierro la conexion de broadcast
  ghs_n_broadcast_neighbor_discovery_exit_handler(list_head(neighbors_list), &linkaddr_node_addr);
}
/*---------------------------------------------------------------------------*/

/* This function is called whenever a broadcast message is received. */
static void n_broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{

  ghs_n_broadcast_recv(list_head(neighbors_list), packetbuf_dataptr(), from,
                       packetbuf_attr(PACKETBUF_ATTR_RSSI),
                       packetbuf_attr(PACKETBUF_ATTR_LINK_QUALITY),
                       &neighbors_memb, neighbors_list );
}
/* This is where we define what function to be called when a broadcast
   is received. We pass a pointer to this structure in the
   broadcast_open() call below. */
static const struct broadcast_callbacks broadcast_call = {n_broadcast_recv};

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

    if ((seqno > STOP_BROADCAST) && !(flags & LINK_WEIGHT_AGREEMENT) )
    {
        process_exit(&broadcast_neighbor_discovery);   //Se cierra el proceso y se llama el PROCESS_EXITHANDLER(funcion)
        process_post(&link_weight_worst_case,PROCESS_EVENT_CONTINUE, NULL); //Inicio el proceso de unicast
    }

    if(flags & LINK_WEIGHT_AGREEMENT)
    {
        etimer_set(&et, CLOCK_SECOND * 60); //Espero a que otros acaben unicast
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
        process_exit(&link_weight_worst_case);   //Se cierra el proceso y se llama el PROCESS_EXITHANDLER(funcion)
    }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/* Este proceso envia mensajes de unicast a los vecinos para
*  ponerse de acuerdo en el peso del link. Se toma el peor caso,
*  es decir, si el peso de A->B es de 1.00 y el peso de B->A es de
*  1.25 entonces B le informa (unicast msg) a A que peso 1.25 debe imponerse
* , entonces A cambia su peso A->B a 1.25
*/
PROCESS_THREAD(link_weight_worst_case, ev, data)
{

  PROCESS_EXITHANDLER(n_link_weight_worst_exit_handler());
  PROCESS_BEGIN();
  unicast_open(&n_uc, 146, &unicast_callbacks);

  while(1)
  {
    static struct etimer et;
    struct neighbor *n_aux;
    struct unicast_message msg;

    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
    etimer_set(&et, CLOCK_SECOND * 60); // Espero 60 segundos a q otros acaben broadcast
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    for(n_aux = list_head(neighbors_list); n_aux != NULL; n_aux = list_item_next(n_aux)) // Recorrer toda la lista
    {
        msg.avg_seqno_gap = n_aux -> avg_seqno_gap;
        packetbuf_copyfrom(&msg, sizeof(msg));
        unicast_send(&n_uc, &n_aux->addr);
    }
    flags |= LINK_WEIGHT_AGREEMENT; // Termine de enviar unicast a todos mis vecinos
    //PROCESS_EXIT(); //Termino con el proceso una vez se halla enviado el msg unicast
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(broadcast_neighbor_discovery, ev, data)
{
  static struct etimer et;
  struct broadcast_message msg;

  PROCESS_EXITHANDLER(n_broadcast_neighbor_discovery_exit_handler());

  PROCESS_BEGIN();

  broadcast_open(&n_broadcast, 129, &broadcast_call);

  while(1) {

    /* Send a broadcast every 16 - 32 seconds */
    etimer_set(&et, CLOCK_SECOND * 16 + random_rand() % (CLOCK_SECOND * 16));

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    msg.seqno = seqno;
    packetbuf_copyfrom(&msg, sizeof(struct broadcast_message));
    broadcast_send(&n_broadcast);
    seqno++;
  }

  PROCESS_END();
}


/*---------------------------------------------------------------------------*/
