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
 /*----------- VARIABLES GLOBALES ---------------------------------------------- */
 /*------------------------------------------------------------------- */

 /*Listas de runicast para saber cual seq ha llegado. Si ha llegado
 * duplicado o no
 */
 MEMB(history_mem, struct history_entry, NUM_HISTORY_ENTRIES);
 LIST(history_list);

 MEMB(report_memb, report_str, MAX_NUM_REPORTS); // Defines a memory pool for edges
 LIST(report_list); // List that holds the neighbors we have seen thus far
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

   ghs_report_ChaRoot_recv_ruc(packetbuf_dataptr() ,list_head(history_list), from, &history_mem,
                        history_list, seqno, &report_memb, report_list, &reports_completos );


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
 static void master_report_ChaRoot_exit_handler(void)
 {
     printf("Process Exit: master_report_ChaRoot \n");
 }

 /*------------------------------------------------------------------- */
 /*-----------PROCESOS------------------------------------------------*/
 /*------------------------------------------------------------------- */


 PROCESS(master_report_ChaRoot, "Proceso master de los msg report-ChangeRoot");
 PROCESS(send_message_report_ChaRoot, "Enviar msg de report-ChangeRoot");
 PROCESS(reports_completos, "Evalua silos hijos ya enviaron los msg de report");


 PROCESS_THREAD(master_report_ChaRoot, ev, data)
{
    PROCESS_EXITHANDLER(master_report_ChaRoot_exit_handler());
    PROCESS_BEGIN();

    e_init_master_report_ChaRoot = process_alloc_event(); // Darle un numero al evento
    e_msg_report                 = process_alloc_event(); // Darle un numero al evento

    //process_start(&reports_completos, NULL);

    while(1)
    {
        PROCESS_WAIT_EVENT(); // Wait for any event.
        if(ev == e_init_master_report_ChaRoot)
        {
            printf("Estoy en el master_report_ChaRoot \n  ");
        }

    }//end of infinite while

    PROCESS_END();
}

PROCESS_THREAD(reports_completos, ev, data)
{
    PROCESS_EXITHANDLER();
    PROCESS_BEGIN();


    while(1)
    {
        PROCESS_WAIT_EVENT(); // Wait for any event.
        if(ev == PROCESS_EVENT_CONTINUE)
        {
            printf("A evaluar reports\n");

            //if(!(nd.flags & CORE_NODE))
            //{
                //Si la lista de reports ya esta completa
                if(list_length(report_list) == nd.num_children)//Si el tamano de la lista es = al num de hijos
                {
                    //Saco el nodo con menor peso de la lista
                    printf("Lista de Reports completa \n");

                    //Encuentro el menor de la lista
                    report_str *rp_str = NULL;
                    uint32_t lowest_weight;
                    report_str *lowest_rp = NULL;
                    for(rp_str = list_head(report_list), lowest_weight = rp_str->rp_msg.weight_r,
                        lowest_rp = rp_str;
                        rp_str != NULL; rp_str = rp_str->next)
                    {
                        if(rp_str->rp_msg.weight_r < lowest_weight)
                        {
                            lowest_weight = rp_str->rp_msg.weight_r;
                            lowest_rp     = rp_str;
                        }
                    }

                    linkaddr_copy( &nd.downroute , &lowest_rp->neighbor);
                    linkaddr_copy(&nd.lwoe.children.neighbor, &lowest_rp->rp_msg.neighbor_r );
                    nd.lwoe.children.weight = lowest_rp->rp_msg.weight_r;
                    nd.flags |= CH_LWOE; //Ya encontre el ND_LWOE

                    printf("El menor de la lista es %d weight=%d.%02d nd.flags=%04X\n",
                    nd.lwoe.children.neighbor.u8[0],
                    (int)(nd.lwoe.children.weight / SEQNO_EWMA_UNITY),
                    (int)(((100UL * nd.lwoe.children.weight) / SEQNO_EWMA_UNITY) % 100),
                    nd.flags);
                } // si lista == num_children

            /*}else // Si soy core node
            {
                if(list_length(report_list) == (nd.num_children - 1) )//Resto 1 porque el otro
                                                                      //CORE_NODE nunca me va a
                                                                     // enviar un report
                {
                    //Saco el nodo con menor peso de la lista
                    printf("Lista de Reports completa (soy core_node)\n");

                    //Encuentro el menor de la lista
                    report_str *rp_str = NULL;
                    uint32_t lowest_weight;
                    report_str *lowest_rp = NULL;
                    for(rp_str = list_head(report_list), lowest_weight = rp_str->rp_msg.weight_r,
                        lowest_rp = rp_str;
                        rp_str != NULL; rp_str = rp_str->next)
                    {
                        if(rp_str->rp_msg.weight_r < lowest_weight)
                        {
                            lowest_weight = rp_str->rp_msg.weight_r;
                            lowest_rp     = rp_str;
                        }
                    }

                    linkaddr_copy( &nd.downroute , &lowest_rp->neighbor);
                    linkaddr_copy(&nd.lwoe.children.neighbor, &lowest_rp->rp_msg.neighbor_r );
                    nd.lwoe.children.weight = lowest_rp->rp_msg.weight_r;
                    nd.flags |= CH_LWOE; //Ya encontre el ND_LWOE

                    printf("El menor de la lista es %d weight=%d.%02d nd.flags=%04X\n",
                    nd.lwoe.children.neighbor.u8[0],
                    (int)(nd.lwoe.children.weight / SEQNO_EWMA_UNITY),
                    (int)(((100UL * nd.lwoe.children.weight) / SEQNO_EWMA_UNITY) % 100),
                    nd.flags);
                } // si lista == num_children

            }*/
        }
    }

    PROCESS_END();

}



PROCESS_THREAD(send_message_report_ChaRoot, ev, data)
{
    PROCESS_EXITHANDLER();
    PROCESS_BEGIN();

    runicast_open(&runicast, 146, &runicast_callbacks); //el 144-145 ya estan usados

    /* OPTIONAL: Sender history */
    list_init(history_list);
    memb_init(&history_mem);

    while(1)
    {
        static struct etimer et;

        PROCESS_WAIT_EVENT(); // Wait for any event.

        if(ev == e_msg_report)
        {
            static report_msg *rp_msg_d; //rp = report
            rp_msg_d = (report_msg *) data;
            static report_msg rp_msg;

            //Delay 2-4 seconds  //Para que no todos lo manden al tiempo
            etimer_set(&et, CLOCK_SECOND * 2 + random_rand() % (CLOCK_SECOND * 2));
            PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

            if(!runicast_is_transmitting(&runicast)) // Si runicast no esta TX, entra
            {
                llenar_report_msg(&rp_msg, &nd.parent, &rp_msg_d->neighbor_r,
                                  rp_msg_d->weight_r);
                packetbuf_copyfrom(&rp_msg, sizeof(rp_msg));
                packetbuf_set_attr(PACKETBUF_ATTR_PACKET_GHS_TYPE_MSG, REPORT);
                runicast_send(&runicast, &rp_msg.destination, MAX_RETRANSMISSIONS);
                printf("Envie report a %d Neigh=%d Weight=%d.%02d \n",
                        rp_msg.destination.u8[0],
                        rp_msg.neighbor_r.u8[0],
                        (int)( rp_msg.weight_r / SEQNO_EWMA_UNITY),
                        (int)(((100UL * rp_msg.weight_r ) / SEQNO_EWMA_UNITY) % 100) );
            }


        }
    }//end of infinite while

    PROCESS_END();

}
