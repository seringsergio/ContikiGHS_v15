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
 *         Gallagher-Humblet-Spira (GHS) Algorithm
 * \author
 *         Sergio Diaz <diaz-sergio@javeriana.edu.co>
 *
 *         This example implements the GHS algorithm. It
 *         includes:
 *         1. /home/sergiodiaz/Desktop/contiki/core/net/rime/ghs_neigh.h.c
 *
 *
 *
 */


 /*------------------------------------------------------------------- */
 /*----------- INCLUDES ------------------------------------------------ */
 /*------------------------------------------------------------------- */

#include "contiki.h"
#include "lib/list.h"
#include "lib/memb.h"
#include "lib/random.h"
#include "net/rime/rime.h" //Aca esta ghs.h
#include "ghs_algorithm.h"
#include <stdio.h>


/*------------------------------------------------------------------- */
/*----------GLOBAL VARIABLES -----------------------------------------*/
/*------------------------------------------------------------------- */
uint8_t state = DISCOVERY_BROADCAST; // Estado del proceso
uint8_t flags = 0; // Banderas del proceso

MEMB(neighbors_memb, struct neighbor, MAX_NEIGHBORS); // Defines a memory pool for neighbors
LIST(neighbors_list); // List that holds the neighbors we have seen thus far

/*------------------------------------------------------------------- */
/*----------STATIC VARIABLES -----------------------------------------*/
/*------------------------------------------------------------------- */
static struct broadcast_conn n_broadcast; // Es la conexion broadcast.
static struct unicast_conn n_uc; // Es la conexion unicast.

/*------------------------------------------------------------------- */
/*----------PROCESSES------- -----------------------------------------*/
/*------------------------------------------------------------------- */
PROCESS(wait, "Wait for Network Stabilization");

PROCESS(n_broadcast_neighbor_discovery, "Neighbor Discovery via Broadcast");
PROCESS(n_link_weight_worst_case, "Assume Worst Weight for Link");
PROCESS(master_neighbor_discovery, "GHS Control");

//PROCESS(idle, "Idle process");

AUTOSTART_PROCESSES(&master_neighbor_discovery, &n_broadcast_neighbor_discovery,
                     &wait, &n_link_weight_worst_case,&idle_mio);

/*---------------------------------------------------------------------------*/
/* Funcion que recibe un mensaje de unicast: Si el avg_seqno_gap del vecino es
*  mayor, entonces reemplazo mi avg_seqno_gap. Para tener un acuerdo entre el avg_seqno_gap
*  de upward y downward. Se impone el mayor.
*/
static void n_recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{
  ghs_n_recv_uc(list_head(neighbors_list), packetbuf_dataptr(), from );
}
/*---------------------------------------------------------------------------*/
/* Informa que se envio un mensage de unicast
*/
static void n_sent_uc(struct unicast_conn *c, int status, int num_tx)
{
  ghs_n_sent_uc(packetbuf_addr(PACKETBUF_ADDR_RECEIVER), &linkaddr_null, status, num_tx );
}
static const struct unicast_callbacks unicast_callbacks = {n_recv_uc, n_sent_uc};
/*---------------------------------------------------------------------------*/
/* Exit_Handler: Cuando el proceso de ponerse de acuerdo en los pesos de
* upward y downward de los links finaliza, entonces  ordeno la lista de
* vecinos y la imprimo.
*/
static void n_link_weight_worst_exit_handler(void)
{
    unicast_close(&n_uc); // Cierro la conexion unicast
    ghs_n_link_weight_worst_exit_handler(list_head(neighbors_list), &linkaddr_node_addr);
}
/*---------------------------------------------------------------------------*/
/* Cuando se termina el proceso de conocer a los vecinos por broadcast, entonces
*  se imprime la lista
*/
static void n_broadcast_neighbor_discovery_exit_handler(void)
{
  broadcast_close(&n_broadcast); // Cierro la conexion de broadcast
  ghs_n_broadcast_neighbor_discovery_exit_handler(list_head(neighbors_list), &linkaddr_node_addr);
}
/*---------------------------------------------------------------------------*/
/* Cada vez que llega un broadcast, se evalua si el vecino ya existe, si no existe
*  se crea y se agrega a la lista de vecinos, para el vecino se actualiza: rssi, lqi y seqno;
*  tambien se calcula el nuevo avg_seqno_gap.
*/
static void n_broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{

  ghs_n_broadcast_recv(list_head(neighbors_list), packetbuf_dataptr(), from,
                       packetbuf_attr(PACKETBUF_ATTR_RSSI),
                       packetbuf_attr(PACKETBUF_ATTR_LINK_QUALITY),
                       &neighbors_memb, neighbors_list );
}
static const struct broadcast_callbacks broadcast_call = {n_broadcast_recv};

/*------------------------------------------------------------------- */
/*-----------PROCESOS------------------------------------------------*/
/*------------------------------------------------------------------- */

/*---------------------------------------------------------------------------*/
/* Este es el proceso MASTER. Este proceso controla a los otros procesos esclavos.
*  Los puede controlar con start, stop, exit, continue, etc. Es decir, este proceso
*  le indica a los otros cuando correr y cuando detenerse
*/

PROCESS_THREAD(master_neighbor_discovery, ev, data)
{
  PROCESS_BEGIN();

  static struct process *last_process = NULL; //Ultimo proceso que se ejecuto
  static uint8_t seconds;

  while(1)
  {

    if(state == DISCOVERY_BROADCAST)
    {
        process_post(&n_broadcast_neighbor_discovery,PROCESS_EVENT_CONTINUE, NULL);
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
        last_process = data;
        state = WAIT_NETWORK_STABILIZATION;
    }else
    if(state == WAIT_NETWORK_STABILIZATION)
    {
        if(last_process == &n_broadcast_neighbor_discovery)
        {
            seconds = 30;
            process_post(&wait, PROCESS_EVENT_CONTINUE, &seconds);
            PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
            process_exit(&n_broadcast_neighbor_discovery);   //Se cierra el proceso y se llama el PROCESS_EXITHANDLER(funcion)
            state = WEIGHT_WORST;

        }else if(last_process == &n_link_weight_worst_case)
        {
            seconds = 30;
            process_post(&wait, PROCESS_EVENT_CONTINUE, &seconds);
            PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
            process_exit(&n_link_weight_worst_case);   //Se cierra el proceso y se llama el PROCESS_EXITHANDLER(funcion)
            state = IDLE;
        }
    }else
    if(state == WEIGHT_WORST)
    {
        process_post(&n_link_weight_worst_case,PROCESS_EVENT_CONTINUE, NULL); //Inicio el proceso de unicast
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
        last_process = data;
        state = WAIT_NETWORK_STABILIZATION;
    }else
    if(state == IDLE)
    {
        //PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
        process_post(&idle_mio,PROCESS_EVENT_CONTINUE, NULL); //Inicio el proceso de unicast
        printf("LLAMO0000 a idle_mio \n");
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
        printf("NUNCA DEBE IMPRIMIRSE ESTO \n");
    }
}//End of while
  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
/* Envio mensaje de broadcast para conocer a mis vecinos.
*/PROCESS_THREAD(n_broadcast_neighbor_discovery, ev, data)
{
  PROCESS_EXITHANDLER(n_broadcast_neighbor_discovery_exit_handler());
  PROCESS_BEGIN();

  static struct etimer et;
  static uint8_t seqno;
  struct broadcast_message msg;


  broadcast_open(&n_broadcast, 129, &broadcast_call);

  PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);

  while(1)
  {

    /* Delay 2-4 seconds */
    etimer_set(&et, CLOCK_SECOND * 2 + random_rand() % (CLOCK_SECOND * 2));
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    /* En msg incluye el secuence number para saber cuantos msg se
    *  pierden en promedio (exponentially-weighted moving average)
    */
    msg.seqno = seqno;
    packetbuf_copyfrom(&msg, sizeof(struct broadcast_message));
    broadcast_send(&n_broadcast);
    seqno++;

    if(seqno > STOP_BROADCAST)
    {
        process_post(&master_neighbor_discovery,PROCESS_EVENT_CONTINUE, PROCESS_CURRENT()); //Inicio el proceso de unicast
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE); //Nunca debe llegar este evento
        printf("NUNCA DEBE IMPRIMIRSE ESTO \n");
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
PROCESS_THREAD(n_link_weight_worst_case, ev, data)
{

  PROCESS_EXITHANDLER(n_link_weight_worst_exit_handler());
  PROCESS_BEGIN();

  unicast_open(&n_uc, 146, &unicast_callbacks);

  PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);

  while(1)
  {
    static struct etimer et;
    static struct neighbor *n_aux = NULL;
    struct unicast_message msg;

    /*Envio mensaje de unicast a todos mis vecinos informando mi peso = avg_seqno_gap*/
    for(n_aux = list_head(neighbors_list); n_aux != NULL; n_aux = list_item_next(n_aux)) // Recorrer toda la lista
    {
        /* Delay 2-4 seconds */
        etimer_set(&et, CLOCK_SECOND * 2 + random_rand() % (CLOCK_SECOND * 2));
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
        msg.avg_seqno_gap = n_aux->avg_seqno_gap;
        packetbuf_copyfrom(&msg, sizeof(msg));
        unicast_send(&n_uc, &n_aux->addr);
    }

    process_post(&master_neighbor_discovery,PROCESS_EVENT_CONTINUE, PROCESS_CURRENT()); //Inicio el proceso de unicast
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE); //Nunca debe llegar este evento
    printf("NUNCA DEBE IMPRIMIRSE ESTO \n");

  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/* Espero por X seconds
*/
PROCESS_THREAD(wait, ev, data)
{
    PROCESS_BEGIN();

    static struct etimer et;
    static uint8_t *seconds;

    while(1)
    {
      PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
      seconds = data;

      etimer_set(&et, CLOCK_SECOND * (*seconds) );
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

      process_post(&master_neighbor_discovery,PROCESS_EVENT_CONTINUE, NULL); //Inicio el proceso de unicast
    }
    PROCESS_END();

}
