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

MEMB(edges_memb, edges, MAX_NUM_EDGES); // Defines a memory pool for edges
LIST(edges_list); // List that holds the neighbors we have seen thus far

edges *e_list_head_g; //Es el apuntador a la cabeza de la lista global

/*Listas de runicast para saber cual seq ha llegado. Si ha llegado
* duplicado o no
*/
MEMB(history_mem, struct history_entry, NUM_HISTORY_ENTRIES);
LIST(history_list);


// Lista para guardar los msg de connect
MEMB(co_mem, connect_list , MAX_TAMANO_LISTA_MSG);
LIST(co_list);

// Lista para guardar los msg de initiate
MEMB(i_mem,  initiate_list   , MAX_TAMANO_LISTA_MSG);
LIST(i_list);


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
                    co_list, &co_mem, &evaluar_msg_co,
                   i_list, &i_mem , &evaluar_msg_i, &evaluar_msg_test);

}
static void sent_runicast(struct runicast_conn *c, const linkaddr_t *to, uint8_t retransmissions)
{
  printf("runicast (co-i) message sent to %d.%d, retransmissions %d\n",
     to->u8[0], to->u8[1], retransmissions);
}
static void
timedout_runicast(struct runicast_conn *c, const linkaddr_t *to, uint8_t retransmissions)
{
    printf("runicast (co-i) message timed out when sending to %d.%d, retransmissions %d\n",
     to->u8[0], to->u8[1], retransmissions);
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
PROCESS(evaluar_msg_co, "Evaluar Mensaje de Connect");
PROCESS(evaluar_msg_i, "Evaluar Mensaje de initiate");


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

    //proceso report - ChangeRoot

        e_msg_report          = process_alloc_event(); // Darle un numero al evento
        e_msg_ch_root         = process_alloc_event(); // Darle un numero al evento


    static s_wait str_wait;
    while(1)
    {
        PROCESS_WAIT_EVENT(); // Wait for any event.
        if (ev == e_init_master_co_i)
        {

            static connect_msg c_msg;

            //Terminar procesos
            process_exit(&master_neighbor_discovery);   //Se cierra el proceso y se llama el PROCESS_EXITHANDLER(funcion)
            //Iniciar procesos co_i
            process_start(&send_message_co_i, NULL);
            process_start(&evaluar_msg_co, NULL );
            process_start(&evaluar_msg_i, NULL );
            //Procesos de test ar
            process_start(&send_message_test_ar, NULL);
            process_start(&e_test, NULL);
            process_start(&evaluar_msg_test, NULL);
            process_start(&evaluar_msg_accept, NULL);
            //procesos de report-ChangeRoot
            process_start(&evaluar_msg_rp, NULL); //para inicializar report_list_g y report_memb_g
            process_start(&send_message_report_ChaRoot, NULL); //para inicializar report_list_g y report_memb_g
            process_start(&e_LWOE, NULL); //para inicializar report_list_g y report_memb_g
            process_start(&evaluar_msg_cr, NULL); //para inicializar report_list_g y report_memb_g

            //Inicializar el master_co_i
            init_master_co_i(data, &edges_memb, edges_list);

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
        if (ev == e_found)
        {
            //Espero instrucciones de change_root o initiate
            printf("Estoy en FOUND \n");

            //termino GHS
            /*printf("2. Acabo GHS algorithm Name=%d.%02d\n",
            (int)(nd.f.name / SEQNO_EWMA_UNITY),
            (int)(((100UL * nd.f.name) / SEQNO_EWMA_UNITY) % 100));*/

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
            static connect_msg co_msg;

            if(!runicast_is_transmitting(&runicast)) // Si runicast no esta TX, entra
            {
                llenar_connect_msg (&co_msg, c_msg_d->level, &c_msg_d->destination);
                packetbuf_copyfrom(&co_msg, sizeof(co_msg));
                packetbuf_set_attr(PACKETBUF_ATTR_PACKET_GHS_TYPE_MSG, CONNECT);
                runicast_send(&runicast, &co_msg.destination, MAX_RETRANSMISSIONS);
                printf("Envio CONECT to %d , level=%d \n", co_msg.destination.u8[0], co_msg.level);
            }else //Si runicast esta ocupado TX, pospongo el envio del msg
            {
                //pospone sending the message
                llenar_connect_msg (&co_msg, c_msg_d->level, &c_msg_d->destination);
                process_post(PROCESS_CURRENT(), e_msg_connect, &co_msg);
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
            }else //Si runicast esta ocupado TX, pospongo el envio del msg
            {
                //pospone sending the message
                llenar_initiate_msg(&i_msg, msg_d->f.name, msg_d->f.level,
                                    msg_d->nd_state,  &msg_d->destination, msg_d->flags );
                process_post(PROCESS_CURRENT(), e_msg_initiate, &i_msg);
            }
        }
    } //END of while
    PROCESS_END();
}

PROCESS_THREAD(evaluar_msg_co, ev, data)
{
    PROCESS_BEGIN();

    // Iniciar la lista
    list_init(co_list);
    memb_init(&co_mem);

    while(1)
    {
        PROCESS_WAIT_EVENT(); // Wait for any event.
        if(ev == PROCESS_EVENT_CONTINUE)
        {
            if(list_length(co_list))
            {
                connect_list *co_list_p;
                for(co_list_p = list_head(co_list); co_list_p != NULL; co_list_p = co_list_p->next)
                {
                    static initiate_msg i_msg;
                    //connect_msg *co_msg = (connect_msg *) msg;

                    if(co_list_p->co_msg.level == nd.f.level) //Si los dos fragmentos tienen el mismo nivel
                    {
                        if(state_is_branch(&co_list_p->from, e_list_head_g)) // Caso inicial. Fragmentos con 1 nodo
                        {
                            printf("Tamano lista=%d Evaluo ConNect de %d con level=%d \n",
                            list_length(co_list),
                             co_list_p->from.u8[0],
                             co_list_p->co_msg.level);

                            nd.num_children = nd.num_children + 1;
                            nd.flags |= CORE_NODE;
                            linkaddr_copy(&nd.otro_core_node, &co_list_p->from);


                            llenar_initiate_msg(&i_msg, weight_with_edge(&co_list_p->from, e_list_head_g),
                                                (nd.f.level + 1), FIND, &co_list_p->from, BECOME_CORE_NODE);
                            process_post(&send_message_co_i,  e_msg_initiate, &i_msg); //Hijo + 1 !!
                            //remuevo el elemento de la lista
                            my_list_remove(co_list, co_list_p); //Remove a specific element from a list.
                            memb_free(&co_mem, co_list_p);

                        }else //Si el estado NO es branch (El proceso postpones processing CONECT)
                        {
                            printf("Tamano lista=%d Pospone ConNect de %d con level=%d \n",
                            list_length(co_list),
                             co_list_p->from.u8[0],
                             co_list_p->co_msg.level);

                            list_remove(co_list, co_list_p); //Remove a specific element from a list.
                            list_add(co_list, co_list_p); //Add an item at the end of a list.

                        } //FIN de pospone connect
                    }else
                    if(co_list_p->co_msg.level < nd.f.level)
                    {
                        printf("Tamano lista=%d Evaluo ConNect de %d con level=%d \n",
                        list_length(co_list),
                         co_list_p->from.u8[0],
                         co_list_p->co_msg.level);

                        become_branch(e_list_head_g, &co_list_p->from); // become branch de connect
                        nd.num_children = nd.num_children + 1;
                        llenar_initiate_msg(&i_msg, nd.f.name, nd.f.level, nd.state, &co_list_p->from, ~BECOME_CORE_NODE);
                        process_post(&send_message_co_i,  e_msg_initiate, &i_msg); //Hijo + 1 !!

                        //remuevo el elemento de la lista
                        my_list_remove(co_list, co_list_p); //Remove a specific element from a list.
                        memb_free(&co_mem, co_list_p);

                    }
                } //END for para recorrer lista
            } // END Si la lista tiene elementos
        } //END PROCESS EV==CONTINUE
    } //END OF WHILE
    PROCESS_END();
} //END of PROCESS THREAD

PROCESS_THREAD(evaluar_msg_i, ev, data)
{

    PROCESS_BEGIN();

    // Iniciar la lista
    list_init(i_list);
    memb_init(&i_mem);

    while(1)
    {
        PROCESS_WAIT_EVENT(); // Wait for any event.
        if(ev == PROCESS_EVENT_CONTINUE)
        {
            if(list_length(i_list))
            {
                initiate_list *i_list_p;
                for(i_list_p = list_head(i_list); i_list_p != NULL; i_list_p = i_list_p->next)
                {
                    //initiate_msg *i_msg = (initiate_msg *) msg;
                    static initiate_msg i_msg_d;

                    nd.f.name  = i_list_p->i_msg.f.name;
                    nd.f.level = i_list_p->i_msg.f.level;
                    nd.state   = i_list_p->i_msg.nd_state;
                    linkaddr_copy(&nd.parent , &i_list_p->from);

                    if(i_list_p->i_msg.flags & BECOME_CORE_NODE)
                    {
                        linkaddr_copy(&nd.otro_core_node, &i_list_p->from);
                        nd.flags |= CORE_NODE;
                        printf("Soy CORE_NORE 2\n");
                    }

                    if(i_list_p->i_msg.nd_state == FIND) //si cambio de estado a FIND
                    {
                        //Envio un mensaje al master_co_i de find
                        process_post(&master_co_i,  e_find, NULL);
                        nd.state = FIND;  //Para saber en que estado estoy en cualquier parte
                    }else
                    if(i_list_p->i_msg.nd_state == FOUND) //si cambio de estado a FOUND
                    {
                        //Envio un mensaje al master_co_i de found
                        process_post(&master_co_i,  e_found, NULL);
                        nd.state = FOUND;  //Para saber en que estado estoy en cualquier parte
                    }

                    //Reenvio el msg por todas las BRANCHES
                    edges *e_aux;
                    for(e_aux = e_list_head_g; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
                    {
                        //Propagar el INITIATE por las otras ramas
                        //Si es una BRANCH y no es el nodo que me envio el INITIATE (No le devuelvo el msg)
                        if( (e_aux->state == BRANCH) && !linkaddr_cmp(&e_aux->addr, &i_list_p->from))
                        {
                            //nd.num_children = nd.num_children + 1;
                            llenar_initiate_msg(&i_msg_d, i_list_p->i_msg.f.name, i_list_p->i_msg.f.level,
                                               i_list_p->i_msg.nd_state, &e_aux->addr, ~BECOME_CORE_NODE);
                            process_post(&send_message_co_i,  e_msg_initiate, &i_msg_d);
                        }
                    }

                    printf("TamanoLista =%d llego INITIATE from %d.%d name=%d.%02d level=%d state=%d parent=%d\n",
                          list_length(i_list),
                          i_list_p->from.u8[0], i_list_p->from.u8[1],
                          (int)(nd.f.name / SEQNO_EWMA_UNITY),
                          (int)(((100UL * nd.f.name) / SEQNO_EWMA_UNITY) % 100),
                          nd.f.level,
                          nd.state,
                          nd.parent.u8[0]);

                  //Remuevo el elemento de la lista
                  my_list_remove(i_list, i_list_p); //Remove a specific element from a list.
                  memb_free(&i_mem, i_list_p);

                } //FOR todos los elementos de la lista
            } //Si hay elementos en la lista
        } //END IF  EV == CONTINUE
    } //end of while
    PROCESS_END();
} //END of PROCESS THREAD evaluar_msg_i
