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
 *         Gallagher-Humblet-Spira (GHS) Algorithm:
 *         Neighbor Discovery
 * \author
 *         Sergio Diaz <diaz-sergio@javeriana.edu.co>
 *
 *         This example implements the GHS algorithm. It
 *         includes:
 *         1. /home/sergiodiaz/Desktop/contiki/core/net/rime/ghs_neigh.h.c
 *         2. /home/sergiodiaz/Desktop/contiki/core/net/rime/ghs_co_i.h.c
 *         3. /home/sergiodiaz/Desktop/contiki/core/net/rime/ghs_test_ar.h.c
 *         4. /home/sergiodiaz/Desktop/contiki/core/net/rime/ghs_report_ChaRoot.h.c
 *
 *  \Comentarios
 *  1. Esta basado en el codigo rime/example-neighbors.c
 *     https://github.com/contiki-os/contiki/blob/master/examples/rime/example-neighbors.c
 */

 /*------------------------------------------------------------------- */
 /*----------- INCLUDES ------------------------------------------------ */
 /*------------------------------------------------------------------- */

#include "contiki.h"
#include "lib/list.h"
#include "lib/memb.h"
#include "lib/random.h"
#include "net/rime/rime.h" //Aca esta ghs_neigh.h
#include "ghs_algorithm.h"
#include <stdio.h>
#include "dev/leds.h"

/*------------------------------------------------------------------- */
/*----------GLOBAL VARIABLES -----------------------------------------*/
/*------------------------------------------------------------------- */
uint8_t flags = 0; // Banderas del proceso

MEMB(neighbors_memb, struct neighbor, MAX_NEIGHBORS); // Defines a memory pool for neighbors
LIST(neighbors_list); // List that holds the neighbors we have seen thus far

/*Listas de runicast para saber cual seq ha llegado. Si ha llegado
* duplicado o no
*/
MEMB(history_mem, struct history_entry, NUM_HISTORY_ENTRIES);
LIST(history_list);

/*------------------------------------------------------------------- */
/*----------STATIC VARIABLES -----------------------------------------*/
/*------------------------------------------------------------------- */
static struct broadcast_conn n_broadcast; // Es la conexion broadcast.
static struct runicast_conn runicast_worst_case; //Es la conexion de runicast

/*------------------------------------------------------------------- */
/*----------PROCESSES------- -----------------------------------------*/
/*------------------------------------------------------------------- */
PROCESS(wait, "Wait for Network Stabilization");

PROCESS(n_broadcast_neighbor_discovery, "Neighbor Discovery via Broadcast");
PROCESS(n_link_weight_worst_case, "Assume Worst Weight for Link");
PROCESS(master_neighbor_discovery, "GHS Control");
//PROCESS(energy_measurement, "Energy measurement");
//PROCESS(DataCollection, "Data Collection - Send packet 30seconds"); // Process that sends a packet every 30 seconds to its parent

AUTOSTART_PROCESSES( //&energy_measurement ,
                    &master_neighbor_discovery, &n_broadcast_neighbor_discovery,
                     &wait, &n_link_weight_worst_case,
                     &master_co_i
                     //, &DataCollection
                     );

/*------------------------------------------------------------------- */
/*-----------FUNCIONES-------------------------------------------------*/
/*------------------------------------------------------------------- */
/*---------------------------------------------------------------------------*/
/* Funcion que recibe un mensaje de runicast: Si el avg_seqno_gap del vecino es
*  mayor, entonces reemplazo mi avg_seqno_gap. Para tener un acuerdo entre el avg_seqno_gap
*  de upward y downward. Se impone el mayor.
*/
static void recv_runicast(struct runicast_conn *c, const linkaddr_t *from, uint8_t seqno)
{
  ghs_n_recv_ruc(list_head(neighbors_list), packetbuf_dataptr(), from,
                 &history_mem, history_list, seqno);
}
static void sent_runicast(struct runicast_conn *c, const linkaddr_t *to, uint8_t retransmissions)
{
  MY_DBG_3("runicast message sent to %d.%d, retransmissions %d\n",
     to->u8[0], to->u8[1], retransmissions);
}
static void
timedout_runicast(struct runicast_conn *c, const linkaddr_t *to, uint8_t retransmissions)
{
  MY_DBG_1("ERROR: runicast message timed out when sending to %d.%d, retransmissions %d\n",
     to->u8[0], to->u8[1], retransmissions);
}
static const struct runicast_callbacks runicast_callbacks = {recv_runicast,
							     sent_runicast,
							     timedout_runicast};
/*---------------------------------------------------------------------------*/
/* Exit_Handler: Cuando el proceso de ponerse de acuerdo en los pesos de
* upward y downward de los links finaliza, entonces  ordeno la lista de
* vecinos y la imprimo.
*/
static void n_link_weight_worst_exit_handler(void)
{
    runicast_close(&runicast_worst_case);
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
/* Exit handler del proceso master_neighbor_discovery
*/
//----------------------Unicast para el data collection------------
/*
static void
recv_uc_data_collection(struct unicast_conn *c, const linkaddr_t *from)
{
  //MY_DEBUG_2
  printf("unicast message received from %d.%d - Data Collection\n",
	 from->u8[0], from->u8[1]);
}
//
static void
sent_uc_data_collection(struct unicast_conn *c, int status, int num_tx)
{
  const linkaddr_t *dest = packetbuf_addr(PACKETBUF_ADDR_RECEIVER);
  if(linkaddr_cmp(dest, &linkaddr_null)) {
    return;
  }
  //MY_DEBUG_2
  printf("unicast message sent to %d.%d: status %d num_tx %d - Data Collection\n",
    dest->u8[0], dest->u8[1], status, num_tx);
}

static const struct unicast_callbacks unicast_callbacks = {recv_uc_data_collection, sent_uc_data_collection};  //Unicast para el data collection
static struct unicast_conn uc; //Conexion de unicast para el data collection */
/*---------------------------------------------------------------------------*/

static void master_neighbor_discovery_exit_handler(void)
{
    MY_DBG_3("Process Exit: master_neighbor_discovery \n");
}
/*------------------------------------------------------------------- */
/*-----------PROCESOS------------------------------------------------*/
/*------------------------------------------------------------------- */
/*---------------------------------------------------------------------------*/

/*PROCESS_THREAD(DataCollection, ev, data)
{
    //static struct etimer periodic;
    //static struct etimer et;

    PROCESS_BEGIN();
    unicast_open(&uc, 146, &unicast_callbacks);

    // Allow some time for the network to settle - Esperar a que el arbol se construya -El timepo de convergencia de GHS es 160 segundos segun mediciones
    etimer_set(&et, 160 * CLOCK_SECOND);
    PROCESS_WAIT_UNTIL(etimer_expired(&et));

    while(1)
    {

        // Send a packet every 30 seconds.
        if(etimer_expired(&periodic))
        {
            etimer_set(&periodic, CLOCK_SECOND * 30);
            etimer_set(&et, random_rand() % (CLOCK_SECOND * 30));
        }

        PROCESS_WAIT_EVENT();

        if(etimer_expired(&et))
        {
            packetbuf_copyfrom("Hello", 5);
            //if(!linkaddr_cmp(&nd.parent, &linkaddr_node_addr)) { //Al comienzo yo soy mi padre. Es decir, parent = linkaddr_node_addr
              unicast_send(&uc, &nd.parent);
            //}
        }

      }//END WHILE
      PROCESS_END();
} //END PROCESS*/

/*PROCESS_THREAD(energy_measurement, ev, data)
{


    PROCESS_BEGIN();

    //Quitar el comentario para medir la energia
    //powertrace_start(CLOCK_SECOND * 10);

    PROCESS_END();
}*/

/* Este es el proceso MASTER. Este proceso controla a los otros procesos esclavos.
*  Los puede controlar con start, stop, exit, continue, etc. Es decir, este proceso
*  le indica a los otros cuando correr y cuando detenerse
*/
PROCESS_THREAD(master_neighbor_discovery, ev, data)
{
  PROCESS_EXITHANDLER(master_neighbor_discovery_exit_handler());
  PROCESS_BEGIN();

  //Definir eventos: Comunes a todos los procesos
  e_wait_stabilization = process_alloc_event(); // Darle un numero al evento
  //e_infinite_wait = process_alloc_event(); // Darle un numero al evento

  //Definir eventos: neighbor discovery
  e_discovery_broadcast = process_alloc_event(); // Darle un numero al evento
  e_weight_worst        = process_alloc_event(); // Darle un numero al evento
  e_init_master_co_i    = process_alloc_event(); // Darle un numero al evento

  process_post(PROCESS_CURRENT(), e_discovery_broadcast, NULL);

  static struct process *last_process = NULL; //Ultimo proceso que se ejecuto
  static s_wait str_wait;

  while(1)
  {

    PROCESS_WAIT_EVENT(); // Wait for any event.
    if(ev == e_discovery_broadcast)
    {
        process_post(&n_broadcast_neighbor_discovery,PROCESS_EVENT_CONTINUE, NULL);
    }else
    if(ev == e_wait_stabilization) //Espero hasta q se envien los broadcast o unicast
    {
        last_process = (struct process *) data;
        if(last_process == &n_broadcast_neighbor_discovery)
        {
            llenar_wait_struct(&str_wait, WAIT_NETWORK_STABILIZATION, PROCESS_CURRENT()  );
            process_post(&wait, PROCESS_EVENT_CONTINUE, &str_wait);
            PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
            process_exit(&n_broadcast_neighbor_discovery);   //Se cierra el proceso y se llama el PROCESS_EXITHANDLER(funcion)
            process_post(PROCESS_CURRENT(), e_weight_worst, NULL);

        }else if(last_process == &n_link_weight_worst_case)
        {
            llenar_wait_struct(&str_wait, WAIT_NETWORK_STABILIZATION, PROCESS_CURRENT()  );
            process_post(&wait, PROCESS_EVENT_CONTINUE, &str_wait);
            PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
            process_exit(&n_link_weight_worst_case);   //Se cierra el proceso y se llama el PROCESS_EXITHANDLER(funcion)
            process_post(PROCESS_CURRENT(), e_init_master_co_i, NULL);
        }
    }else
    if(ev == e_weight_worst)
    {
        process_post(&n_link_weight_worst_case,PROCESS_EVENT_CONTINUE, NULL); //Inicio el proceso de runicast
    }else
    if(ev == e_init_master_co_i)
    {
        process_post(&master_co_i, e_init_master_co_i, list_head(neighbors_list));
    }
}//End of while
  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
/* Envio mensaje de broadcast para conocer a mis vecinos.
*/
PROCESS_THREAD(n_broadcast_neighbor_discovery, ev, data)
{
  PROCESS_EXITHANDLER(n_broadcast_neighbor_discovery_exit_handler());
  PROCESS_BEGIN();

  broadcast_open(&n_broadcast, 129, &broadcast_call);

  static struct etimer et;
  static uint8_t seqno;
  struct broadcast_message msg;

  while(1)
  {
      PROCESS_WAIT_EVENT(); // Wait for any event.
      if(ev == PROCESS_EVENT_CONTINUE)
      {
          seqno = 0;
          while(seqno <= STOP_BROADCAST)
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
            MY_DBG_3("Este es broadcast vecinos\n");
            seqno++;
          }
          process_post(&master_neighbor_discovery,e_wait_stabilization, PROCESS_CURRENT());
      }//END IF CONTINUE
  }//END of whie

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
/* Este proceso envia mensajes de runicast a los vecinos para
*  ponerse de acuerdo en el peso del link. Se toma el peor caso,
*  es decir, si el peso de A->B es de 1.00 y el peso de B->A es de
*  1.25 entonces B le informa (runicast msg) a A que peso 1.25 debe imponerse
* , entonces A cambia su peso A->B a 1.25
*/
PROCESS_THREAD(n_link_weight_worst_case, ev, data)
{

  PROCESS_EXITHANDLER(n_link_weight_worst_exit_handler());
  PROCESS_BEGIN();

  runicast_open(&runicast_worst_case, 144, &runicast_callbacks);

  /* OPTIONAL: Sender history */
  list_init(history_list);
  memb_init(&history_mem);

  static struct etimer et;
  static struct neighbor *n_aux = NULL;
  struct runicast_message msg;

  while(1)
  {
    PROCESS_WAIT_EVENT(); // Wait for any event.
    if(ev == PROCESS_EVENT_CONTINUE)
    {
        /*Envio mensaje de runicast a todos mis vecinos informando mi peso = avg_seqno_gap*/
        for(n_aux = list_head(neighbors_list); n_aux != NULL; n_aux = n_aux->next) // Recorrer toda la lista
        {
            /* Delay 2-4 seconds */
            etimer_set(&et, CLOCK_SECOND * 2 + random_rand() % (CLOCK_SECOND * 2));
            PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

            if(!runicast_is_transmitting(&runicast_worst_case)) // Si runicast no esta TX, entra
            {
                msg.avg_seqno_gap = n_aux->avg_seqno_gap; //llenar el msg
                packetbuf_copyfrom(&msg, sizeof(msg));
                MY_DBG_3("%u.%u: sending runicast to address %u.%u\n",
                   linkaddr_node_addr.u8[0],
                   linkaddr_node_addr.u8[1],
                   n_aux->addr.u8[0],
                   n_aux->addr.u8[1]);
                runicast_send(&runicast_worst_case, &n_aux->addr, MAX_RETRANSMISSIONS);
                while (runicast_is_transmitting(&runicast_worst_case))
                {
                    PROCESS_PAUSE();
                }
            }else
            {
                //Espero que esto no pase porque tengo una espera de 2 a 4 seg entre msgs
                // ademas tengo un while
                MY_DBG_1("ERROR: El runicast esta ocupado - No envie msg de AGREE ON LINK WEIGHT\n");
            }
        } //END of FOR
        process_post(&master_neighbor_discovery,e_wait_stabilization, PROCESS_CURRENT());
    }//END if ev == CONTINUE
  }//END of while
  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
/* Espero por X seconds
*/
PROCESS_THREAD(wait, ev, data)
{
    PROCESS_BEGIN();

    static struct etimer et;
    static s_wait *str_wait;

    while(1)
    {
          PROCESS_WAIT_EVENT(); // Wait for any event.
          if(ev == PROCESS_EVENT_CONTINUE)
          {
              str_wait = (s_wait *) data;

              etimer_set(&et, CLOCK_SECOND * str_wait->seconds );
              PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

              //process_post_synch(str_wait->return_process,PROCESS_EVENT_CONTINUE, NULL);
              process_post(str_wait->return_process,PROCESS_EVENT_CONTINUE, NULL);

          }
    }
    PROCESS_END();
}
