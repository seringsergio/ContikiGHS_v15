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
    //msg
    e_msg_test = process_alloc_event(); // Darle un numero al evento
    e_msg_reject = process_alloc_event(); // Darle un numero al evento
    e_msg_accept = process_alloc_event(); // Darle un numero al evento


    while(1)
    {

        PROCESS_WAIT_EVENT(); // Wait for any event.
        if(ev == e_init_master_test_ar)
        {
            static pass_info_test_ar *str_t_ar;
            str_t_ar = (pass_info_test_ar *) data;

            init_master_test_ar(str_t_ar->master_co_i, &send_message_test_ar, &e_pospone_test);

            edges *e_list_head = list_head(str_t_ar->edges_list);
            edges *e_aux;

            for(e_aux = e_list_head; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
            {

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

    printf("Process Init: send_message_test_ar \n");
    while(1)
    {
        PROCESS_WAIT_EVENT(); // Wait for any event.
        if(ev == e_msg_test)
        {

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

    printf("Process Init: e_pospone_test \n");
    while(1)
    {
        PROCESS_WAIT_EVENT(); // Wait for any event.
    }

    PROCESS_END();
}
