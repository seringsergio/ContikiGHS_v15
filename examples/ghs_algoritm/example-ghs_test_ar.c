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
 *         4. /home/sergiodiaz/Desktop/contiki/core/net/rime/ghs_report_ChaRoot.h
 */
 /*------------------------------------------------------------------- */
 /*----------- INCLUDES ---------------------------------------------- */
 /*------------------------------------------------------------------- */
#include "contiki.h"
#include "lib/list.h"
#include "lib/memb.h"
#include "lib/random.h"
#include "net/rime/rime.h" //Aca esta ghs.h
#include "ghs_algorithm.h"
#include <stdio.h>
/*------------------------------------------------------------------- */
/*----------PROCESSES------- -----------------------------------------*/
/*------------------------------------------------------------------- */
PROCESS(send_message_test_ar, "Enviar msg de test-accept-reject");
PROCESS(e_pospone_test, "Evaluar Pospone Test");
PROCESS(e_test, "Evaluar con Test Neighbors");

/*------------------------------------------------------------------- */
/*----------- VARIABLES GLOBALES ---------------------------------------------- */
/*------------------------------------------------------------------- */

/*Listas de runicast para saber cual seq ha llegado. Si ha llegado
* duplicado o no
*/
MEMB(history_mem, struct history_entry, NUM_HISTORY_ENTRIES);
LIST(history_list);

//pt = pospone test
MEMB(pt_memb, pospone_test, MAX_NUM_POSPONES); // Defines a memory pool for edges
LIST(pt_list); // List that holds the neighbors we have seen thus far


/*------------------------------------------------------------------- */
/*----------STATIC VARIABLES -----------------------------------------*/
/*------------------------------------------------------------------- */
static struct runicast_conn runicast; //Es la conexion de runicast
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------- */
/*-----------FUNCIONES-------------------------------------------------*/
/*------------------------------------------------------------------- */
static void
recv_runicast(struct runicast_conn *c, const linkaddr_t *from, uint8_t seqno)
{

  ghs_test_ar_recv_ruc(packetbuf_dataptr() ,list_head(history_list), from, &history_mem,
                       history_list, seqno, &send_message_test_ar, e_list_head_g,
                       pt_list, &pt_memb, &e_test, &send_message_report_ChaRoot,
                       &master_co_i, &e_LWOE    );


}
static void
sent_runicast(struct runicast_conn *c, const linkaddr_t *to, uint8_t retransmissions)
{
  printf("runicast TEEEST message sent to %d.%d, retransmissions %d\n",
	 to->u8[0], to->u8[1], retransmissions);
}
static void
timedout_runicast(struct runicast_conn *c, const linkaddr_t *to, uint8_t retransmissions)
{
  printf("runicast TEEEST message timed out when sending to %d.%d, retransmissions %d\n",
	 to->u8[0], to->u8[1], retransmissions);
}
static const struct runicast_callbacks runicast_callbacks = {recv_runicast,
							     sent_runicast,
							     timedout_runicast};



/*------------------------------------------------------------------- */
/*-----------PROCESOS------------------------------------------------*/
/*------------------------------------------------------------------- */

/* Proceso para testear los vecinos con msg de TEST
*/
PROCESS_THREAD(e_test, ev, data)
{
    PROCESS_EXITHANDLER();
    PROCESS_BEGIN();

    //static s_wait str_wait;

    while(1)
    {
        //static struct etimer et;

        PROCESS_WAIT_EVENT(); // Wait for any event.
        if (ev == PROCESS_EVENT_CONTINUE)
        {
            printf("CONTINUE e_test level=%d\n ", nd.f.level);

            char string[] = "REAAD";

            print_edges_list(e_list_head_g, string,  &linkaddr_node_addr);

            uint8_t tengo_edges_de_salida = 0;
            edges *e_aux;
            test_msg t_msg;
            for(e_aux = e_list_head_g; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
            {
                if(e_aux->state == BASIC)
                {
                    llenar_test_msg(&t_msg, &e_aux->addr, nd.f );
                    process_post(&send_message_test_ar, e_msg_test, &t_msg);
                    tengo_edges_de_salida = 1;
                    break; //Envio msg TEST al primer BASIC. Recordar que la lista esta ordenada
                    //PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
                }
            }

            if(tengo_edges_de_salida == 0)
            {
                //NO BORRAR!
                //printf("No tengo_edges_de_salida. INFINITO = %" PRIu32 "\n", INFINITO);
                printf("No tengo edge de salida \n");
                nd.lwoe.node.weight = INFINITO;
                nd.flags |= ND_LWOE; //Ya encontre el ND_LWOE. Porque no tengo edges de salida
                process_post(&e_LWOE, PROCESS_EVENT_CONTINUE, NULL);

            }

        } //end IF CONTINUE
    } //end of while

    PROCESS_END();
}



/* Proceso que envia msg de test - accept - reject
*/
PROCESS_THREAD(send_message_test_ar, ev, data)
{
    PROCESS_EXITHANDLER();
    PROCESS_BEGIN();

    runicast_open(&runicast, 145, &runicast_callbacks); //el 144 ya esta usado

    /* OPTIONAL: Sender history */
    list_init(history_list);
    memb_init(&history_mem);

    //printf("Process Init: send_message_test_ar \n");
    while(1)
    {
        //static struct etimer et;

        PROCESS_WAIT_EVENT(); // Wait for any event.
        if(ev == e_msg_test)
        {
            static test_msg *t_msg_d;
            t_msg_d = (test_msg *) data;
            test_msg t_msg;

            if(!runicast_is_transmitting(&runicast)) // Si runicast no esta TX, entra
            {
                llenar_test_msg(&t_msg, &t_msg_d->destination, t_msg_d->f);
                packetbuf_copyfrom(&t_msg, sizeof(t_msg));
                packetbuf_set_attr(PACKETBUF_ATTR_PACKET_GHS_TYPE_MSG, TEST);
                runicast_send(&runicast, &t_msg.destination, MAX_RETRANSMISSIONS);
                printf("Deseo enviar e_msg_test a %d\n", t_msg.destination.u8[0]);

            }
        }else
        if(ev == e_msg_reject)
        {
            static reject_msg *r_msg_d;
            r_msg_d = (reject_msg *) data;
            static reject_msg r_msg;

            if(!runicast_is_transmitting(&runicast)) // Si runicast no esta TX, entra
            {
                llenar_reject_msg(&r_msg, &r_msg_d->destination);

                become_rejected(e_list_head_g, &r_msg.destination);

                packetbuf_copyfrom(&r_msg, sizeof(r_msg));
                packetbuf_set_attr(PACKETBUF_ATTR_PACKET_GHS_TYPE_MSG, M_REJECT);
                runicast_send(&runicast, &r_msg.destination, MAX_RETRANSMISSIONS);
                printf("Envie reject a %d \n",r_msg.destination.u8[0]);
            }

        }else
        if(ev == e_msg_accept)
        {

            static accept_msg *a_msg_d;
            a_msg_d = (accept_msg *) data;
            static accept_msg a_msg;

            if(!runicast_is_transmitting(&runicast)) // Si runicast no esta TX, entra
            {
                llenar_accept_msg(&a_msg, &a_msg_d->destination);
                packetbuf_copyfrom(&a_msg, sizeof(a_msg));
                packetbuf_set_attr(PACKETBUF_ATTR_PACKET_GHS_TYPE_MSG, M_ACCEPT);
                runicast_send(&runicast, &a_msg.destination, MAX_RETRANSMISSIONS);
                printf("Envie accept a %d \n",a_msg.destination.u8[0]);
            }
        }
    } //end of while

    PROCESS_END();
}

/* Proceso que evalua los msg pendientes de test
*/
PROCESS_THREAD(e_pospone_test, ev, data)
{
    PROCESS_EXITHANDLER();
    PROCESS_BEGIN();

    //printf("Process Init: e_pospone_test \n");
    while(1)
    {
        //static struct etimer et;
        static pospone_test *pt_aux = NULL;
        static accept_msg a_msg;
        static reject_msg r_msg;


        /*etimer_set(&et, CLOCK_SECOND * 1); //Evaluo msg de test pendientes cada 1 seg
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));*/

        PROCESS_WAIT_EVENT(); // Wait for any event.
        if(ev == PROCESS_EVENT_CONTINUE)
        {
            if( list_length (pt_list) ) //Si hay elementos en la lista
            {
                for(pt_aux = list_head(pt_list); pt_aux != NULL; pt_aux = list_item_next(pt_aux)) // Recorrer toda la lista
                {
                    if(pt_aux->t_msg.f.level > nd.f.level)
                    {
                        //Pospones processing the incomming test msg, until (t_msg->f.level < nd.f.level)
                        //No lo voy a posponer otra vez!
                    }else
                    if(pt_aux->t_msg.f.level <= nd.f.level)
                    {
                        if(pt_aux->t_msg.f.name == nd.f.name)
                        {
                            //Enviar reject
                            llenar_reject_msg (&r_msg, &pt_aux->neighbor);
                            process_post(&send_message_test_ar, e_msg_reject, &r_msg);
                            printf("Quuuiero enviar e_msg_reject a %d \n", r_msg.destination.u8[0]);
                            //Como se soluciona el test se remueve de la lista
                            list_remove (pt_list, pt_aux);

                        }else
                        {
                            //Enviar accept
                            llenar_accept_msg (&a_msg, &pt_aux->neighbor);
                            process_post(&send_message_test_ar, e_msg_accept, &a_msg);
                            printf("Quuuiero enviar e_msg_accept a %d \n", a_msg.destination.u8[0]);
                            //Como se soluciona el test se remueve de la lista
                            list_remove (pt_list, pt_aux);
                        }
                    }
                } //For cada elemento de la lista
            }//end if hay elementos en la lista
        } //END IF CONTINUE
    } //end of while
    PROCESS_END();
}
