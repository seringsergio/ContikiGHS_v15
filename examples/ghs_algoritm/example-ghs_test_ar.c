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
PROCESS(e_test, "Evaluar con Test Neighbors");
PROCESS(evaluar_msg_test, "Evaluar Mensaje de Test");
PROCESS(evaluar_msg_accept, "Evaluar Mensaje de Accept");
PROCESS(evaluar_msg_reject, "Evaluar Mensaje de Reject");

/*------------------------------------------------------------------- */
/*----------- VARIABLES GLOBALES ---------------------------------------------- */
/*------------------------------------------------------------------- */

/*Listas de runicastt para saber cual seq ha llegado. Si ha llegado
* duplicado o no
*/
MEMB(history_mem, struct history_entry, NUM_HISTORY_ENTRIES);
LIST(history_list);

// Lista para guardar los msg de test
MEMB(t_mem,  test_list   , MAX_TAMANO_LISTA_MSG);
LIST(t_list);

// Lista para guardar los msg de test
MEMB(t_mem_out,  test_list   , MAX_TAMANO_LISTA_MSG);
LIST(t_list_out);

list_t t_list_out_g;
struct memb *t_mem_out_g;

// Lista para guardar los msg de accept
MEMB(a_mem,  accept_list   , MAX_TAMANO_LISTA_MSG);
LIST(a_list);

// Lista para guardar los msg de accept
MEMB(a_mem_out,  accept_list   , MAX_TAMANO_LISTA_MSG);
LIST(a_list_out);

list_t a_list_out_g;
struct memb *a_mem_out_g;

// Lista para guardar los msg de reject
MEMB(rj_mem,  reject_list   , MAX_TAMANO_LISTA_MSG);
LIST(rj_list);

// Lista para guardar los msg de reject
MEMB(rj_mem_out,  reject_list   , MAX_TAMANO_LISTA_MSG);
LIST(rj_list_out);

list_t rj_list_out_g;
struct memb *rj_mem_out_g;

uint8_t cont_e_pos_t_msg;  //Contador para evaluar los pospone solo X veces
/*------------------------------------------------------------------- */
/*----------STATIC VARIABLES -----------------------------------------*/
/*------------------------------------------------------------------- */
static struct runicast_conn runicast; //Es la conexion de runicastt
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------- */
/*-----------FUNCIONES-------------------------------------------------*/
/*------------------------------------------------------------------- */
static void
recv_runicast(struct runicast_conn *c, const linkaddr_t *from, uint8_t seqno)
{
    void *msg = packetbuf_dataptr();

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
       MY_DBG("runicast message received from %d.%d, seqno %d (DUPLICATE)\n",
 	     from->u8[0], from->u8[1], seqno);
       return;
     }
     /* Update existing history entry */
     e->seq = seqno;
   }
      //Leer el packet buffer attribute: Especificamente el tipo de mensaje
      packetbuf_attr_t msg_type = packetbuf_attr(PACKETBUF_ATTR_PACKET_GHS_TYPE_MSG);
      // Evaluo el tipo de msg que llego
      if(msg_type == TEST)
      {
          cont_e_pos_t_msg = 0;

          test_list *t_list_p;
          t_list_p = memb_alloc(&t_mem); //Alocar memoria
          if(t_list_p == NULL)
          {
              MY_DBG("ERROR: La lista de msg de test esta llena\n");
          }else
          {
              t_list_p->t_msg = *((test_msg *)msg); //msg le hago cast.Luego cojo todo el msg
              linkaddr_copy(&t_list_p->from, from);
              list_add(t_list, t_list_p); //Add an item at the end of a list.
              process_post_synch(&evaluar_msg_test, PROCESS_EVENT_CONTINUE, NULL ) ;
          }
      }else
      if(msg_type == M_ACCEPT)
      {
          accept_list *a_list_p;
          a_list_p = memb_alloc(&a_mem); //Alocar memoria
          if(a_list_p == NULL)
          {
              MY_DBG("ERROR: La lista de msg de accept esta llena\n");
          }else
          {
              linkaddr_copy(&a_list_p->from, from);
              list_add(a_list, a_list_p); //Add an item at the end of a list.
              process_post_synch(&evaluar_msg_accept, PROCESS_EVENT_CONTINUE, NULL);
          }
      }else
      if(msg_type == M_REJECT)
      {
          reject_list *rj_list_p;
          rj_list_p = memb_alloc(&rj_mem); //Alocar memoria
          if(rj_list_p == NULL)
          {
              MY_DBG("ERROR: La lista de msg de Reject esta llena\n");
          }else
          {
              linkaddr_copy(&rj_list_p->from, from);
              list_add(rj_list, rj_list_p); //Add an item at the end of a list.
              process_post_synch(&evaluar_msg_reject, PROCESS_EVENT_CONTINUE, NULL);
          }
      }

}
static void
sent_runicast(struct runicast_conn *c, const linkaddr_t *to, uint8_t retransmissions)
{
  MY_DBG("runicastt (test-ar) message sent to %d.%d, retransmissions %d\n",
	 to->u8[0], to->u8[1], retransmissions);
}
static void
timedout_runicast(struct runicast_conn *c, const linkaddr_t *to, uint8_t retransmissions)
{
  MY_DBG("runicastt (test-ar) message timed out when sending to %d.%d, retransmissions %d\n",
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
    PROCESS_BEGIN();

    uint8_t tengo_edges_de_salida = 0;
    static char string[] = "REAAD";
    static edges *e_aux = NULL;
    static test_list *t_list_out_p;

    while(1)
    {
        //static struct etimer et;
        PROCESS_WAIT_EVENT(); // Wait for any event.
        if (ev == PROCESS_EVENT_CONTINUE)
        {
            MY_DBG("CONTINUE e_test level=%d\n ", nd.f.level);
            print_edges_list(e_list_head_g, string,  &linkaddr_node_addr);

            tengo_edges_de_salida = 0;
            for(e_aux = e_list_head_g; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
            {
                //un rejected nunca se vuelve a testear
                if( (e_aux->state == BASIC) || (e_aux->state == E_ACCEPTED) )
                {
                    MY_DBG("addr=%d e_aux->state =%d \n",e_aux->addr.u8[0], e_aux->state);

                    //send TEST msg
                    t_list_out_p = memb_alloc(&t_mem_out); //Alocar memoria
                    llenar_test_msg_list(t_list_out_p, &e_aux->addr, nd.f );
                    list_add(t_list_out, t_list_out_p); //Add an item at the end of a list
                    process_post(&send_message_test_ar, e_msg_test, NULL);

                    tengo_edges_de_salida = 1;
                    break; //Envio msg TEST al primer BASIC. Recordar que la lista esta ordenada
                    //PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
                }
            }

            if(tengo_edges_de_salida == 0)
            {
                //NO BORRAR! : MY_DBG("No tengo_edges_de_salida. INFINITO = %" PRIu32 "\n", INFINITO);
                MY_DBG("No tengo edge de salida \n");
                nd.lwoe.node.weight = INFINITO;
                nd.flags |= ND_LWOE; //Ya encontre el ND_LWOE. Porque no tengo edges de salida
                process_post_synch(&e_LWOE, PROCESS_EVENT_CONTINUE, NULL);
            }
        } //end IF CONTINUE
    } //end of while
    PROCESS_END();
}

PROCESS_THREAD(evaluar_msg_test, ev, data)
{
    PROCESS_BEGIN();

    // Iniciar la lista
    list_init(t_list);
    memb_init(&t_mem);

    static test_list *t_list_p;
    static accept_list *a_list_out_p;
    static reject_list *rj_list_out_p;

    while(1)
    {
        PROCESS_WAIT_EVENT(); // Wait for any event.
        if(ev == PROCESS_EVENT_CONTINUE)
        {
            if(list_length(t_list))
            {
                for(t_list_p = list_head(t_list); t_list_p != NULL; t_list_p = t_list_p->next)
                {
                     if(t_list_p->t_msg.f.level > nd.f.level)
                     {
                         MY_DBG("TL=%d  Pospone un TEsT msg from %d con name=%d.%02d, level=%d > nd.f.level=%d \n",
                         list_length(t_list),
                         t_list_p->from.u8[0],
                         (int)(t_list_p->t_msg.f.name_str.weight / SEQNO_EWMA_UNITY),
                         (int)(((100UL * t_list_p->t_msg.f.name_str.weight) / SEQNO_EWMA_UNITY) % 100),
                         t_list_p->t_msg.f.level,
                         nd.f.level);

                         //tengo q volver al mismo proceso porque el
                         // apuntador a next va a quedar en NULL.
                         // Si no es el ultimo, lo vuelvo a llamar
                         if( (t_list_p->next != NULL) && (cont_e_pos_t_msg < list_length(t_list)) )
                         {
                             cont_e_pos_t_msg = cont_e_pos_t_msg + 1;
                             MY_DBG("Vuelvo a llamar el evaluar test  \n");
                             //OJO: aca el llamado no es synch
                             process_post(PROCESS_CURRENT(), PROCESS_EVENT_CONTINUE, NULL );
                         }
                         //EL Apuntador a next queda = a NULL
                         //Se sale del for -- por eso el anterior post aaaasynchrono.
                         //Ademas. Pospones processing the incomming test msg, until (t_msg->f.level < nd.f.level)
                         list_remove(t_list, t_list_p); //Remove a specific element from a list.
                         list_add(t_list, t_list_p); //Add an item at the end of a list.

                     }else
                     if(t_list_p->t_msg.f.level <= nd.f.level)
                     {
                         if(  nombres_iguales(  &(t_list_p->t_msg.f.name_str) , &(nd.f.name_str)   )   )
                         {
                             MY_DBG("TL=%d  evaluo un TEsT msg from %d con name=%d.%02d, level=%d > nd.f.level=%d \n",
                             list_length(t_list),
                             t_list_p->from.u8[0],
                             (int)(t_list_p->t_msg.f.name_str.weight / SEQNO_EWMA_UNITY),
                             (int)(((100UL * t_list_p->t_msg.f.name_str.weight) / SEQNO_EWMA_UNITY) % 100),
                             t_list_p->t_msg.f.level,
                             nd.f.level);

                             //Enviar reject
                             rj_list_out_p = memb_alloc(&rj_mem_out); //Alocar memoria
                             llenar_reject_msg_list(rj_list_out_p, &t_list_p->from);
                             list_add(rj_list_out, rj_list_out_p); //Add an item at the end of a list
                             process_post(&send_message_test_ar, e_msg_reject, NULL);

                             MY_DBG("Quuuiero enviar e_msg_reject a %d \n", rj_list_out_p->rj_msg.destination.u8[0]);

                             //Remover el dato de la lista
                             //Cuando lo saco de la lista con list_remove() el next es NULL
                             //Por eso creo mi propio my_list_remove(), donde el next no es NULL
                             my_list_remove(t_list, t_list_p); //Remove a specific element from a list.
                             memb_free(&t_mem, t_list_p);

                         }else
                         {
                             MY_DBG("TL=%d  evaluo un TEsT msg from %d con name=%d.%02d, level=%d > nd.f.level=%d \n",
                             list_length(t_list),
                             t_list_p->from.u8[0],
                             (int)(t_list_p->t_msg.f.name_str.weight / SEQNO_EWMA_UNITY),
                             (int)(((100UL * t_list_p->t_msg.f.name_str.weight) / SEQNO_EWMA_UNITY) % 100),
                             t_list_p->t_msg.f.level,
                             nd.f.level);

                             //Enviar accept
                             a_list_out_p = memb_alloc(&a_mem_out); //Alocar memoria
                             llenar_accept_msg_list(a_list_out_p, &t_list_p->from);
                             list_add(a_list_out, a_list_out_p); //Add an item at the end of a list
                             process_post(&send_message_test_ar, e_msg_accept, NULL);

                             //Remover el dato de la lista
                             //Cuando lo saco de la lista con list_remove() el next es NULL
                             //Por eso creo mi propio my_list_remove(), donde el next no es NULL
                             my_list_remove(t_list, t_list_p); //Remove a specific element from a list.
                             memb_free(&t_mem, t_list_p);
                             MY_DBG("Quuuiero enviar e_msg_accept a %d \n", a_list_out_p->a_msg.destination.u8[0]);
                         }
                     }
                } //FOR todos los elementos de la lista - EVALUAR
            } //Si hay elementos en la lista
        } //END IF EV == CONTINUE
    } //END OF WHILE
    PROCESS_END();
} //END OF THREAD


PROCESS_THREAD(evaluar_msg_accept, ev, data)
{

    PROCESS_BEGIN();

    // Iniciar la lista
    list_init(a_list);
    memb_init(&a_mem);
    static accept_list *a_list_p;

    while(1)
    {
        PROCESS_WAIT_EVENT(); // Wait for any event.
        if(ev == PROCESS_EVENT_CONTINUE)
        {
            if(list_length(a_list))
            {
                for(a_list_p = list_head(a_list); a_list_p != NULL; a_list_p = a_list_p->next)
                {
                    MY_DBG("llego AcCept de %d.  flags=%04X \n ",
                    a_list_p->from.u8[0], nd.flags);

                    become_accepted(e_list_head_g, &a_list_p->from);

                    //Si un edges es aceptado: Se guarda el edge como mejor opcion del Nodo
                    linkaddr_copy(&nd.lwoe.node.neighbor,  &a_list_p->from);
                    nd.lwoe.node.weight = return_weight(e_list_head_g, &a_list_p->from);
                    nd.flags |= ND_LWOE; //Ya encontre el ND_LWOE
                    process_post_synch(&e_LWOE, PROCESS_EVENT_CONTINUE, NULL);

                    //Remuevo el elemento de la lista
                    my_list_remove(a_list, a_list_p); //Remove a specific element from a list.
                    memb_free(&a_mem, a_list_p);

                } //FOR cada elemento de la lista
            } //Si hay elementos en la lista
        } //END OF EV== CONTINUE
    } //END of WHILE

    PROCESS_END();

} // END OF PROCESS THREAD

PROCESS_THREAD(evaluar_msg_reject, ev, data)
{
    PROCESS_BEGIN();

    // Iniciar la lista
    list_init(rj_list);
    memb_init(&rj_mem);

    static reject_list *rj_list_p;

    while(1)
    {
        PROCESS_WAIT_EVENT(); // Wait for any event.
        if(ev == PROCESS_EVENT_CONTINUE)
        {
            if(list_length(rj_list))
            {
                for(rj_list_p = list_head(rj_list); rj_list_p != NULL; rj_list_p = rj_list_p->next)
                {
                    if(state_is_branch(&rj_list_p->from,  e_list_head_g))
                    {
                        MY_DBG("Llego REJECT de %d. Pero no puedo asignar el estado YA SOY BRANCH\n"
                              , rj_list_p->from.u8[0]);

                    }else
                    {
                        MY_DBG("Asumo Reject q llego  de %d \n", rj_list_p->from.u8[0]);
                        become_rejected(e_list_head_g, &rj_list_p->from);
                    }

                    //Si el edge es rechazado, entonces testeo uno nuevo.
                    process_post_synch(&e_test , PROCESS_EVENT_CONTINUE, NULL);

                    //remuevo el elemento de la lista
                    my_list_remove(rj_list, rj_list_p); //Remove a specific element from a list.
                    memb_free(&rj_mem, rj_list_p);
                }
            }
        } //IF ev==CONTINUE
    } //End of while
    PROCESS_END();
}

/* Proceso que envia msg de test - accept - reject
*/
PROCESS_THREAD(send_message_test_ar, ev, data)
{
    PROCESS_BEGIN();

    process_start(&e_test, NULL);
    process_start(&evaluar_msg_test, NULL);
    process_start(&evaluar_msg_accept, NULL);
    process_start(&evaluar_msg_reject, NULL);

    runicast_open(&runicast, 145, &runicast_callbacks); //el 144 ya esta usado

    //proceso test_ar
    //estados
    e_evaluate_test       = process_alloc_event(); // Darle un numero al evento
    e_nd_lwoe             = process_alloc_event(); // Darle un numero al evento
    e_ch_lwoe             = process_alloc_event(); // Darle un numero al evento
    //msg
    e_msg_test            = process_alloc_event(); // Darle un numero al evento
    e_msg_reject          = process_alloc_event(); // Darle un numero al evento
    e_msg_accept          = process_alloc_event(); // Darle un numero al evento

    /* OPTIONAL: Sender history */
    list_init(history_list);
    memb_init(&history_mem);

    list_init(t_list_out);
    memb_init(&t_mem_out);

    list_init(a_list_out);
    memb_init(&a_mem_out);

    list_init(rj_list_out);
    memb_init(&rj_mem_out);

    rj_list_out_g = rj_list_out;
    rj_mem_out_g  = &rj_mem_out;

    t_list_out_g = t_list_out;
    t_mem_out_g  = &t_mem_out;

    a_list_out_g = a_list_out;
    a_mem_out_g  = &a_mem_out;

    static test_msg t_msg;
    static accept_msg a_msg;
    static reject_msg r_msg;
    static test_list *t_list_out_p;
    static accept_list *a_list_out_p;
    static reject_list *rj_list_out_p;

    while(1)
    {
        PROCESS_WAIT_EVENT(); // Wait for any event.
        if(ev == e_msg_test)
        {
            if(list_length(t_list_out))
            {
                for(t_list_out_p = list_head(t_list_out); t_list_out_p != NULL; t_list_out_p = t_list_out_p->next)
                {
                    if(!runicast_is_transmitting(&runicast)) // Si runicastt no esta TX, entra
                    {
                        llenar_test_msg(&t_msg, &t_list_out_p->t_msg.destination,
                                        t_list_out_p->t_msg.f);
                        packetbuf_copyfrom(&t_msg, sizeof(t_msg));
                        packetbuf_set_attr(PACKETBUF_ATTR_PACKET_GHS_TYPE_MSG, TEST);
                        runicast_send(&runicast, &t_msg.destination, MAX_RETRANSMISSIONS);

                        MY_DBG("Deseo enviar e_msg_test a %d\n", t_msg.destination.u8[0]);
                        //remuevo el elemento de la lista
                        my_list_remove(t_list_out, t_list_out_p); //Remove a specific element from a list.
                        memb_free(&t_mem_out, t_list_out_p);

                    }else //Si runicastt esta ocupado TX, pospongo el envio del msg
                    {
                        //pospone sending the message
                        list_remove(t_list_out, t_list_out_p); //Remove a specific element from a list.
                        list_add(t_list_out, t_list_out_p);//	Add an item at the end of a list.
                        process_post(PROCESS_CURRENT(), e_msg_test, NULL);
                        //MY_DBG("posponer el msg de test de %d\n", t_list_out_p->t_msg.destination.u8[0]);

                    }
                } //END FOR
            } //END if hay elementos en la lista

        }else
        if(ev == e_msg_reject)
        {
            if(list_length(rj_list_out))
            {
                for(rj_list_out_p = list_head(rj_list_out); rj_list_out_p != NULL; rj_list_out_p = rj_list_out_p->next)
                {
                    if(!runicast_is_transmitting(&runicast)) // Si runicastt no esta TX, entra
                    {
                        llenar_reject_msg(&r_msg, &rj_list_out_p->rj_msg.destination);
                        packetbuf_copyfrom(&r_msg, sizeof(r_msg));
                        packetbuf_set_attr(PACKETBUF_ATTR_PACKET_GHS_TYPE_MSG, M_REJECT);
                        runicast_send(&runicast, &r_msg.destination, MAX_RETRANSMISSIONS);
                        MY_DBG("Envie reject a %d\n",r_msg.destination.u8[0] );

                        //remuevo el elemento de la lista
                        my_list_remove(rj_list_out, rj_list_out_p); //Remove a specific element from a list.
                        memb_free(&rj_mem_out, rj_list_out_p);

                    }
                    else
                    {
                        //pospone sending the message
                        list_remove(rj_list_out, rj_list_out_p); //Remove a specific element from a list.
                        list_add(rj_list_out, rj_list_out_p); //Add an item at the end of a list.
                        process_post(PROCESS_CURRENT(), e_msg_reject, NULL);
                        //MY_DBG("posponer el msg de reject de %d\n", rj_list_out_p->rj_msg.destination.u8[0]);
                    }
                } //END for
            } //END if hay elementos en la lista

        }else
        if(ev == e_msg_accept)
        {
            if(list_length(a_list_out))
            {
                for(a_list_out_p = list_head(a_list_out); a_list_out_p != NULL; a_list_out_p = a_list_out_p->next)
                {
                    if(!runicast_is_transmitting(&runicast)) // Si runicastt no esta TX, entra
                    {
                        llenar_accept_msg(&a_msg, &a_list_out_p->a_msg.destination);
                        packetbuf_copyfrom(&a_msg, sizeof(a_msg));
                        packetbuf_set_attr(PACKETBUF_ATTR_PACKET_GHS_TYPE_MSG, M_ACCEPT);
                        runicast_send(&runicast, &a_msg.destination, MAX_RETRANSMISSIONS);

                        MY_DBG("Envie accept a %d \n",a_msg.destination.u8[0]);
                        //remuevo el elemento de la lista
                        my_list_remove(a_list_out, a_list_out_p); //Remove a specific element from a list.
                        memb_free(&a_mem_out, a_list_out_p);

                    }else //Si runicastt esta ocupado TX, pospongo el envio del msg
                    {
                        //pospone sending the message
                        list_remove(a_list_out, a_list_out_p); //Remove a specific element from a list.
                        list_add(a_list_out, a_list_out_p); 	//Add an item at the end of a list.
                        process_post(PROCESS_CURRENT(), e_msg_accept, NULL);
                        //MY_DBG("posponer el msg de accept de %d\n", a_list_out_p->a_msg.destination.u8[0]);
                    }
                } //END for
            } //END IF hay elementos en la lista
        }
    } //end of while

    PROCESS_END();
}
