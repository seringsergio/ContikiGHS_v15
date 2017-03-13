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
/*----------- VARIABLES GLOBALES ---------------------------------------------- */
/*------------------------------------------------------------------- */
edges *e_list_head_g; //Es el apuntador a la cabeza de la lista global

/*Listas de runicast para saber cual seq ha llegado. Si ha llegado
* duplicado o no
*/
MEMB(history_mem, struct history_entry, NUM_HISTORY_ENTRIES);
LIST(history_list);

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
  /* OPTIONAL: Sender history */
  struct history_entry *e = NULL;
  for(e = list_head(history_list); e != NULL; e = e->next) {
    if(linkaddr_cmp(&e->addr, from)) { // Si las dir son iguales entra
      break;
    }
  }
  if(e == NULL) {
    /* Create new history entry */
    e = memb_alloc(&history_mem);
    if(e == NULL) {
      e = list_chop(history_list); /* Remove oldest at full history */
    }
    linkaddr_copy(&e->addr, from);
    e->seq = seqno;
    list_push(history_list, e);
  } else {
    /* Detect duplicate callback */
    if(e->seq == seqno) {
      printf("runicast message received from %d.%d, seqno %d (DUPLICATE)\n",
	     from->u8[0], from->u8[1], seqno);
      return;
    }
    /* Update existing history entry */
    e->seq = seqno;
  }

  printf("runicast TEEEST message received from %d.%d, seqno %d\n",
	 from->u8[0], from->u8[1], seqno);

 //Leer el packet buffer attribute: Especificamente el tipo de mensaje
 packetbuf_attr_t msg_type = packetbuf_attr(PACKETBUF_ATTR_PACKET_GHS_TYPE_MSG);

 // Evaluo el tipo de msg que llego
 if(msg_type == TEST)
 {
     printf("Reeeeeecibi un TEST msg from %d\n",  from->u8[0]);
 }

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

/* Exit handler de master_test_ar
*/
static void master_test_ar_exit_handler(void)
{
    printf("Process Exit: master_test_ar \n");
}
/*------------------------------------------------------------------- */
/*----------PROCESSES------- -----------------------------------------*/
/*------------------------------------------------------------------- */
PROCESS(master_test_ar, "Proceso master de los msg test-accept-reject");
PROCESS(send_message_test_ar, "Enviar msg de accept - reject");
PROCESS(e_pospone_test, "Evaluar Pospone Test");
PROCESS(e_test, "Evaluar con Test Neoghbors");

/*------------------------------------------------------------------- */
/*-----------PROCESOS------------------------------------------------*/
/*------------------------------------------------------------------- */

/* Proceso master que controla el envio de msg de test - accept - reject
*/
PROCESS_THREAD(master_test_ar, ev, data)
{
    PROCESS_EXITHANDLER(master_test_ar_exit_handler());
    PROCESS_BEGIN();

    //estados
    e_init_master_test_ar = process_alloc_event(); // Darle un numero al evento
    e_evaluate_test = process_alloc_event(); // Darle un numero al evento

    //msg
    e_msg_test = process_alloc_event(); // Darle un numero al evento
    e_msg_reject = process_alloc_event(); // Darle un numero al evento
    e_msg_accept = process_alloc_event(); // Darle un numero al evento

    static s_wait str_wait;


    while(1)
    {

        PROCESS_WAIT_EVENT(); // Wait for any event.
        if(ev == e_init_master_test_ar)
        {
            static pass_info_test_ar *str_t_ar;
            str_t_ar = (pass_info_test_ar *) data;

            init_master_test_ar(str_t_ar->master_co_i, &send_message_test_ar,
                                &e_pospone_test, &e_test);

            e_list_head_g = str_t_ar->e_list_head;

            static edges *e_aux;
            for(e_aux = e_list_head_g; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
            {
                printf("addr=%d weight=%d.%02d state=%d \n",
                       e_aux->addr.u8[0],
                       (int)(e_aux->weight / SEQNO_EWMA_UNITY),
                       (int)(((100UL * e_aux->weight) / SEQNO_EWMA_UNITY) % 100),
                       e_aux->state);

            }

            process_post(PROCESS_CURRENT(), e_wait_stabilization, NULL);
        }else
        if (ev == e_wait_stabilization)
        {
            //Esperemos a que la red se estabilice
            //printf("Inicio Coooontinue\n");
            str_wait.seconds = 15;
            str_wait.return_process = PROCESS_CURRENT();
            process_post(&wait, PROCESS_EVENT_CONTINUE, &str_wait);
            PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
            printf("Final Coooontinue\n");

            process_post(PROCESS_CURRENT(), e_evaluate_test, NULL);

        }else
        if(ev == e_evaluate_test)
        {
            process_post(&e_test, PROCESS_EVENT_CONTINUE, NULL);

        }
    }
    PROCESS_END();

}
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
            static edges *e_aux;
            static test_msg t_msg;
            for(e_aux = e_list_head_g; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
            {
                if(e_aux->state == BASIC)
                {
                    // Delay 2-4 seconds
                    //etimer_set(&et, CLOCK_SECOND * 2 + random_rand() % (CLOCK_SECOND * 2));
                    //PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

                    llenar_test_msg(&t_msg, &e_aux->addr, nd.f );
                    process_post(&send_message_test_ar, e_msg_test, &t_msg);
                    //break;
                    //PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
                }
            }

        }
    }

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
        static struct etimer et;

        PROCESS_WAIT_EVENT(); // Wait for any event.
        if(ev == e_msg_test)
        {
            static test_msg *t_msg_d;
            t_msg_d = (test_msg *) data;
            test_msg t_msg;

            /* Delay 4-8 seconds */ //Para que no todos lo manden al tiempo
            etimer_set(&et, CLOCK_SECOND * 4 + random_rand() % (CLOCK_SECOND * 4));
            PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

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

        }else
        if(ev == e_msg_accept)
        {

        }
    }

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
        PROCESS_WAIT_EVENT(); // Wait for any event.
    }

    PROCESS_END();
}
