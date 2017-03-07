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
node nd;

MEMB(edges_memb, edges, MAX_NUM_EDGES); // Defines a memory pool for edges
LIST(edges_list); // List that holds the neighbors we have seen thus far

/*Listas de runicast para saber cual seq ha llegado. Si ha llegado
* duplicado o no
*/
MEMB(history_mem, struct history_entry, NUM_HISTORY_ENTRIES);
LIST(history_list);

/*------------------------------------------------------------------- */
/*----------STATIC VARIABLES -----------------------------------------*/
/*------------------------------------------------------------------- */
static struct runicast_conn runicast; //Es la conexion de runicast
/*------------------------------------------------------------------- */
/*-----------FUNCIONES-------------------------------------------------*/
/*------------------------------------------------------------------- */

/* Funcion que recibe un mensaje de runicast: Si el avg_seqno_gap del vecino es
*  mayor, entonces reemplazo mi avg_seqno_gap. Para tener un acuerdo entre el avg_seqno_gap
*  de upward y downward. Se impone el mayor.
*/
static void recv_runicast(struct runicast_conn *c, const linkaddr_t *from, uint8_t seqno)
{
  ghs_ff_recv_ruc(packetbuf_dataptr(), from, &history_mem, history_list, seqno);
}
static void sent_runicast(struct runicast_conn *c, const linkaddr_t *to, uint8_t retransmissions)
{
  ghs_ff_send_ruc(to, retransmissions);
}
static void
timedout_runicast(struct runicast_conn *c, const linkaddr_t *to, uint8_t retransmissions)
{
  ghs_ff_timedout_ruc(to, retransmissions);

}
static const struct runicast_callbacks runicast_callbacks = {recv_runicast,
							     sent_runicast,
							     timedout_runicast};

/*------------------------------------------------------------------- */
/*----------PROCESSES------- -----------------------------------------*/
/*------------------------------------------------------------------- */

PROCESS(master_find_found, "Proceso master del Find Found");
PROCESS(send_message, "Enviar msg de connect");

/*------------------------------------------------------------------- */
/*-----------PROCESOS------------------------------------------------*/
/*------------------------------------------------------------------- */

/* Proceso master que controla el find y el found
*/
PROCESS_THREAD(master_find_found, ev, data){
    PROCESS_BEGIN();

    /* OPTIONAL: Sender history */
    list_init(edges_list);
    memb_init(&edges_memb);

    //Definir eventos: master find found
    e_found = process_alloc_event(); // Darle un numero al evento
    e_msg_connect = process_alloc_event(); // Darle un numero al evento
    e_msg_initiate = process_alloc_event();  // Darle un numero al evento
    e_msg_test = process_alloc_event(); // Darle un numero al evento
    e_msg_reject = process_alloc_event(); // Darle un numero al evento
    e_msg_accept = process_alloc_event(); // Darle un numero al evento
    e_msg_report = process_alloc_event(); // Darle un numero al evento
    e_msg_change_root = process_alloc_event(); // Darle un numero al evento

    static s_wait str_wait;

    while(1)
    {
        PROCESS_WAIT_EVENT(); // Wait for any event.
        if (ev == e_init_find_found)
        {
            //printf("Process Init: master_find_found \n");
            init_m_find_found(data, &master_neighbor_discovery,
                              &send_message, &nd,
                              &edges_memb, edges_list, &linkaddr_node_addr );

            //Si no espero la lista se imprime mal. Raro
            str_wait.seconds = 20;
            str_wait.return_process = PROCESS_CURRENT();
            process_post(&wait, PROCESS_EVENT_CONTINUE, &str_wait);
            PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);

            process_post(&send_message,  e_msg_connect, &nd.f.level);

            process_post(PROCESS_CURRENT(), e_found, NULL);

        }else
        if (ev == e_found){
            printf("Estoy en FOUND \n");
        }
    }
    PROCESS_END();

}
/* Proceso para enviar mensajes
*/
PROCESS_THREAD(send_message, ev, data)
{
    PROCESS_BEGIN();
    runicast_open(&runicast, 144, &runicast_callbacks); //Open la conexion


    /* OPTIONAL: Sender history */
    list_init(history_list);
    memb_init(&history_mem);

    while(1)
    {
        static struct etimer et;
        static uint8_t *level;

        PROCESS_WAIT_EVENT(); // Wait for any event.
        if(ev == e_msg_connect)
        {
            level = data;
            connect_msg msg;

            /* Delay 2-4 seconds */
            etimer_set(&et, CLOCK_SECOND * 2 + random_rand() % (CLOCK_SECOND * 2));
            PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

            if(!runicast_is_transmitting(&runicast)) // Si runicast no esta TX, entra
            {
                msg.level = *level;
                packetbuf_copyfrom(&msg, sizeof(msg));
                packetbuf_set_attr(PACKETBUF_ATTR_PACKET_GHS_TYPE_MSG, CONNECT);
                runicast_send(&runicast, &nd.lwoe.node.neighbor, MAX_RETRANSMISSIONS);
            }

            //to nd.lwoe.node.neighbor
            //printf("El level es %d \n", *level);
        }else
        if(ev == e_msg_initiate)
        {

        }else
        if(ev == e_msg_test)
        {

        }else
        if(ev == e_msg_reject)
        {

        }else
        if(ev == e_msg_accept)
        {

        }else
        if(ev == e_msg_report)
        {

        }else
        if(ev == e_msg_change_root)
        {

        }

    }


    PROCESS_END();

}
