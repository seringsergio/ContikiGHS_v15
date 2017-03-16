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
 *         2. /home/sergiodiaz/Desktop/contiki/core/net/rime/ghs_co_i.h.c
 *         3. /home/sergiodiaz/Desktop/contiki/core/net/rime/ghs_test_ar.h.c
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
node nd; //nd es node....n es neighbor
pospone_connect pc; //pc = pospone connect

MEMB(edges_memb, edges, MAX_NUM_EDGES); // Defines a memory pool for edges
LIST(edges_list); // List that holds the neighbors we have seen thus far

/*Listas de runicast para saber cual seq ha llegado. Si ha llegado
* duplicado o no
*/
MEMB(history_mem, struct history_entry, NUM_HISTORY_ENTRIES);
LIST(history_list);

//pc = pospone connect
MEMB(pc_memb, pospone_connect, MAX_NUM_POSPONES); // Defines a memory pool for edges
LIST(pc_list); // List that holds the neighbors we have seen thus far
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
  ghs_ff_recv_ruc(packetbuf_dataptr(), from, &history_mem, history_list, seqno, &nd,
                  list_head(edges_list), &send_message_co_i, &pc_memb, pc_list, &master_co_i );
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

/* Exit handler de master_co_i
*/
static void master_co_i_exit_handler(void)
{
    //runicast_close(&runicast);//Cierro la conexion runicast
    //printf("Process Exit: master_co_i \n");
}
/*------------------------------------------------------------------- */
/*----------PROCESSES------- -----------------------------------------*/
/*------------------------------------------------------------------- */
PROCESS(master_co_i, "Proceso master de los msg connect-initiate");
PROCESS(send_message_co_i, "Enviar msg de connect - initiate");
PROCESS(e_pospone_connect, "Evaluar Pospone Connect");
/*------------------------------------------------------------------- */
/*-----------PROCESOS------------------------------------------------*/
/*------------------------------------------------------------------- */

/* Proceso master que controla el find y el found
*/
PROCESS_THREAD(master_co_i, ev, data)
{
    PROCESS_EXITHANDLER(master_co_i_exit_handler());
    PROCESS_BEGIN();

    /* OPTIONAL: Sender history */
    list_init(edges_list);
    memb_init(&edges_memb);

    //Definir eventos: master find found

    //estados
    e_found = process_alloc_event(); // Darle un numero al evento
    e_find = process_alloc_event(); // Darle un numero al evento
    //msg
    e_msg_connect = process_alloc_event(); // Darle un numero al evento
    e_msg_initiate = process_alloc_event();  // Darle un numero al evento

    /* e_msg_report = process_alloc_event(); // Darle un numero al evento
    e_msg_change_root = process_alloc_event(); // Darle un numero al evento*/

    static s_wait str_wait;
    while(1)
    {
        PROCESS_WAIT_EVENT(); // Wait for any event.
        if (ev == e_init_master_co_i)
        {
            static connect_msg c_msg;
            //Inicializar el master_co_i
            init_master_co_i(data, &master_neighbor_discovery,
                              &send_message_co_i, &e_pospone_connect, &nd,
                              &edges_memb, edges_list, &linkaddr_node_addr );

            //Si no espero la lista se imprime mal. Raro
            str_wait.seconds = 20;
            str_wait.return_process = PROCESS_CURRENT();
            process_post(&wait, PROCESS_EVENT_CONTINUE, &str_wait);
            PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);

            llenar_connect_msg (&c_msg, nd.f.level, &nd.lwoe.node.neighbor);
            process_post(&send_message_co_i,  e_msg_connect, &c_msg);

            process_post(PROCESS_CURRENT(), e_found, NULL);
        }else
        if (ev == e_found){
            //Espero instrucciones de change_root o initiate
            printf("Estoy en FOUND \n");
            nd.flags &= ~ND_LWOE;
            nd.flags &= ~CH_LWOE;

        }else
        if (ev == e_find)
        {
            static pass_info_test_ar str_t_ar; //Estructura para enviar info a master_test_ar

            str_wait.seconds = 15;
            str_wait.return_process = PROCESS_CURRENT();
            process_post(&wait, PROCESS_EVENT_CONTINUE, &str_wait);
            PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);

            printf("FIND Coooontinue\n");
            //me voy al proceso test-accep-reject
            //printf("Estoy en FIND \n");
            process_start(&master_test_ar, NULL);
            llenar_str_test_ar(&str_t_ar, edges_list, PROCESS_CURRENT(), list_head(edges_list));
            process_post(&master_test_ar, e_init_master_test_ar, &str_t_ar ) ;

        }
    }
    PROCESS_END();
}
/* Evaluar los mensajes de pospone connect
*/
PROCESS_THREAD(e_pospone_connect, ev, data)
{
    PROCESS_BEGIN();

    while(1)
    {
        static struct etimer et;
        static pospone_connect *pc_aux = NULL;
        static initiate_msg i_msg;

        etimer_set(&et, CLOCK_SECOND * 1); //Evaluo msg de connect pendientes cada 1 seg
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

        if( list_length (pc_list) ) //Si hay elementos en la lista
        {
            for(pc_aux = list_head(pc_list); pc_aux != NULL; pc_aux = list_item_next(pc_aux)) // Recorrer toda la lista
            {
                if(pc_aux->co_msg.level == nd.f.level) //Si los dos fragmentos tienen el mismo nivel
                {
                    if(state_is_branch( &pc_aux->neighbor, list_head(edges_list))) // Caso inicial. Fragmentos con 1 nodo
                    {
                        nd.num_children = nd.num_children + 1;
                        nd.flags |= CORE_NODE;

                        llenar_initiate_msg(&i_msg, weight_with_edge(&pc_aux->neighbor, list_head(edges_list)),
                                           (nd.f.level + 1), FIND, &pc_aux->neighbor, BECOME_CORE_NODE);
                        process_post(&send_message_co_i,  e_msg_initiate, &i_msg);
                        printf("Soy CORE_NORE 1\n");

                        //Pude procesar el pospone connect con exito.
                        //Entonces lo retiro de la lista
                        list_remove (pc_list, pc_aux);
                        printf("Envio msg de INICIATE DESDE POSPONE a %d \n", i_msg.destination.u8[0] );
                    }else //Si el estado NO es branch (El proceso postpones processing CONECT)
                    {
                        //No lo voy a posponer otra vez!
                        //Simplemente no lo renuevo de la lista pc_list
                    }
                }else
                if(pc_aux->co_msg.level < nd.f.level)
                {
                    become_branch(list_head(edges_list), &pc_aux->neighbor);

                    nd.num_children = nd.num_children + 1;

                    llenar_initiate_msg(&i_msg, nd.f.name, nd.f.level, nd.state, &pc_aux->neighbor,~BECOME_CORE_NODE);
                    process_post(&send_message_co_i,  e_msg_initiate, &i_msg);

                    //Pude procesar el pospone connect con exito.
                    //Entonces lo retiro de la lista
                    list_remove (pc_list, pc_aux);

                    printf("Envio msg de INICIATE DESDE POSPONEEE a %d.%d \n",
                    i_msg.destination.u8[0]
                    , i_msg.destination.u8[1]);

                }
            } //For cada elemento de la lista

        } //end if hay elementos en la lista
    }
    PROCESS_END();
}
/* Proceso para enviar mensajes
*/
PROCESS_THREAD(send_message_co_i, ev, data)
{
    PROCESS_BEGIN();
    runicast_open(&runicast, 144, &runicast_callbacks);

    /* OPTIONAL: Sender history */
    list_init(history_list);
    memb_init(&history_mem);

    while(1)
    {
        static struct etimer et;
        //static uint8_t *level;

        PROCESS_WAIT_EVENT(); // Wait for any event.
        if(ev == e_msg_connect)
        {
            static connect_msg *c_msg_d;
            c_msg_d = data;
            connect_msg msg;

            /* Delay 2-4 seconds */
            etimer_set(&et, CLOCK_SECOND * 2 + random_rand() % (CLOCK_SECOND * 2));
            PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

            if(!runicast_is_transmitting(&runicast)) // Si runicast no esta TX, entra
            {
                llenar_connect_msg (&msg, c_msg_d->level, &c_msg_d->destination);
                packetbuf_copyfrom(&msg, sizeof(msg));
                packetbuf_set_attr(PACKETBUF_ATTR_PACKET_GHS_TYPE_MSG, CONNECT);
                runicast_send(&runicast, &msg.destination, MAX_RETRANSMISSIONS);
                //printf("Envio CONECT to %d , level=%d \n", msg.destination.u8[0], msg.level);
            }
            //to nd.lwoe.node.neighbor
            //printf("El level es %d \n", *level);
        }else
        if(ev == e_msg_initiate)
        {
            static initiate_msg *msg_d;
            msg_d = data;
            static initiate_msg  msg;

            llenar_initiate_msg(&msg, msg_d->f.name, msg_d->f.level,
                                msg_d->nd_state,  &msg_d->destination, msg_d->flags );

            // Delay 2-4 seconds
            /*etimer_set(&et, CLOCK_SECOND * 2 + random_rand() % (CLOCK_SECOND * 2));
            PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));*/
            if(!runicast_is_transmitting(&runicast)) // Si runicast no esta TX, entra
            {
                packetbuf_copyfrom(&msg, sizeof(msg));
                packetbuf_set_attr(PACKETBUF_ATTR_PACKET_GHS_TYPE_MSG, INITIATE);
                runicast_send(&runicast, &msg.destination, MAX_RETRANSMISSIONS);
                printf("Envio initiate a %d \n", msg.destination.u8[0]);
            }
        }
    }
    PROCESS_END();
}
