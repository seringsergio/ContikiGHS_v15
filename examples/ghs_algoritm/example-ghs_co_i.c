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
//node nd; //nd es node....n es neighbor
pospone_connect pc; //pc = pospone connect

MEMB(edges_memb, edges, MAX_NUM_EDGES); // Defines a memory pool for edges
LIST(edges_list); // List that holds the neighbors we have seen thus far

edges *e_list_head_g; //Es el apuntador a la cabeza de la lista global

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
  ghs_co_i_recv_ruc(packetbuf_dataptr(), from, &history_mem, history_list, seqno,
                  list_head(edges_list), &send_message_co_i, &pc_memb, pc_list, &master_co_i,
                  &e_pospone_connect, &e_pospone_test );
}
static void sent_runicast(struct runicast_conn *c, const linkaddr_t *to, uint8_t retransmissions)
{
  ghs_co_i_send_ruc(to, retransmissions);
}
static void
timedout_runicast(struct runicast_conn *c, const linkaddr_t *to, uint8_t retransmissions)
{
  ghs_co_i_timedout_ruc(to, retransmissions);

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

    //Proceso co_i
        //estados
        e_found = process_alloc_event(); // Darle un numero al evento
        e_find = process_alloc_event(); // Darle un numero al evento
        //msg
        e_msg_connect = process_alloc_event(); // Darle un numero al evento
        e_msg_initiate = process_alloc_event();  // Darle un numero al evento

    //proceso test_ar
        //estados
        e_evaluate_test       = process_alloc_event(); // Darle un numero al evento
        e_nd_lwoe             = process_alloc_event(); // Darle un numero al evento
        e_ch_lwoe             = process_alloc_event(); // Darle un numero al evento
        //msg
        e_msg_test            = process_alloc_event(); // Darle un numero al evento
        e_msg_reject          = process_alloc_event(); // Darle un numero al evento
        e_msg_accept          = process_alloc_event(); // Darle un numero al evento
        e_msg_ch_root         = process_alloc_event(); // Darle un numero al evento


    e_list_head_g = list_head(edges_list);
    static s_wait str_wait;
    while(1)
    {
        PROCESS_WAIT_EVENT(); // Wait for any event.
        if (ev == e_init_master_co_i)
        {
            static connect_msg c_msg;
            //Inicializar el master_co_i
            init_master_co_i(data, &master_neighbor_discovery,
                              &send_message_co_i, &e_pospone_connect,
                              &edges_memb, edges_list,
                              &reports_completos, &e_LWOE, &send_message_report_ChaRoot,
                              &e_test, &send_message_test_ar, &e_pospone_test);

            //Espero a que todos hayan inicializado la conexion del connect antes de seguir
            //Ademas, SI NO ESPERO LA LISTA SE IMPRIME MAL: RARO X 2
            llenar_wait_struct(&str_wait, WAIT_NETWORK_STABILIZATION, PROCESS_CURRENT()  );
            process_post(&wait, PROCESS_EVENT_CONTINUE, &str_wait);
            PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);

            //Envio Connect INICIAL con level = 0
            llenar_connect_msg (&c_msg, nd.f.level, &nd.lwoe.node.neighbor);
            process_post(&send_message_co_i,  e_msg_connect, &c_msg);

            //Me voy al estado found
            process_post(PROCESS_CURRENT(), e_found, NULL);
            nd.state = FOUND;   //Para saber en que estado estoy en cualquier parte
        }else
        if (ev == e_found){
            //Espero instrucciones de change_root o initiate
            printf("Estoy en FOUND \n");

            //borro la lista de reportes de vecinos
            list_init(report_list_g); //The list will be empty after this function has been called.
            memb_init(report_memb_g);

            //Reinicio variables
            nd.flags &= ~ND_LWOE;
            nd.flags &= ~CH_LWOE;
            //nd.state = FOUND;   //Para saber en que estado estoy en cualquier parte

        }else
        if (ev == e_find)
        {
            printf("Estoy en FIND \n");
            process_post(&e_test, PROCESS_EVENT_CONTINUE, NULL);
        }
    }//END OF WHILE
    PROCESS_END();
}
/* Evaluar los mensajes de pospone connect
*/
PROCESS_THREAD(e_pospone_connect, ev, data)
{
    PROCESS_BEGIN();

    while(1)
    {
        static pospone_connect *pc_aux = NULL;
        static initiate_msg i_msg;

        PROCESS_WAIT_EVENT(); // Wait for any event.
        if(ev == PROCESS_EVENT_CONTINUE)
        {
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
        } //END if ev == CONTINUE
    } //EnD of while
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
        PROCESS_WAIT_EVENT(); // Wait for any event.
        if(ev == e_msg_connect)
        {
            static connect_msg *c_msg_d;
            c_msg_d = (connect_msg *) data;
            connect_msg co_msg;

            if(!runicast_is_transmitting(&runicast)) // Si runicast no esta TX, entra
            {
                llenar_connect_msg (&co_msg, c_msg_d->level, &c_msg_d->destination);
                packetbuf_copyfrom(&co_msg, sizeof(co_msg));
                packetbuf_set_attr(PACKETBUF_ATTR_PACKET_GHS_TYPE_MSG, CONNECT);
                runicast_send(&runicast, &co_msg.destination, MAX_RETRANSMISSIONS);
                printf("Envio CONECT to %d , level=%d \n", co_msg.destination.u8[0], co_msg.level);
            }
        }else
        if(ev == e_msg_initiate)
        {
            static initiate_msg *msg_d;
            msg_d = (initiate_msg *) data;
            static initiate_msg  i_msg;

            if(!runicast_is_transmitting(&runicast)) // Si runicast no esta TX, entra
            {
                llenar_initiate_msg(&i_msg, msg_d->f.name, msg_d->f.level,
                                    msg_d->nd_state,  &msg_d->destination, msg_d->flags );
                packetbuf_copyfrom(&i_msg, sizeof(i_msg));
                packetbuf_set_attr(PACKETBUF_ATTR_PACKET_GHS_TYPE_MSG, INITIATE);
                runicast_send(&runicast, &i_msg.destination, MAX_RETRANSMISSIONS);
                printf("Envio initiate a %d level= %d name=%d.%02d flags=%04X\n", i_msg.destination.u8[0],
                i_msg.f.level,
                (int)(i_msg.f.name / SEQNO_EWMA_UNITY),
                (int)(((100UL * i_msg.f.name) / SEQNO_EWMA_UNITY) % 100),
                 nd.flags);
            }
        }
    } //END of while
    PROCESS_END();
}
