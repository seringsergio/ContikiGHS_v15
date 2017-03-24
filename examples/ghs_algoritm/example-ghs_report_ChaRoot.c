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

 MEMB(rp_mem, report_list, MAX_TAMANO_LISTA_MSG); // Defines a memory pool for edges
 LIST(rp_list); // List that holds the neighbors we have seen thus far

 MEMB(cr_mem, change_root_list, MAX_TAMANO_LISTA_MSG); // Defines a memory pool for edges
 LIST(cr_list); // List that holds the neighbors we have seen thus far


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

   ghs_report_ChaRoot_recv_ruc(packetbuf_dataptr() , from, &history_mem,
                        history_list, seqno, &rp_mem, rp_list, &evaluar_msg_rp,
                        cr_list, &cr_mem,
                        &evaluar_msg_cr);


 }
 static void
 sent_runicast(struct runicast_conn *c, const linkaddr_t *to, uint8_t retransmissions)
 {
   printf("runicast (report-ChaRoot)  message sent to %d.%d, retransmissions %d flags=%04X\n",
 	 to->u8[0], to->u8[1], retransmissions, nd.flags);
 }
 static void
 timedout_runicast(struct runicast_conn *c, const linkaddr_t *to, uint8_t retransmissions)
 {
   printf("runicast (report-ChaRoot) message timed out when sending to %d.%d, retransmissions %d\n",
 	 to->u8[0], to->u8[1], retransmissions);
 }
 static const struct runicast_callbacks runicast_callbacks = {recv_runicast,
 							     sent_runicast,
 							     timedout_runicast};


 /*------------------------------------------------------------------- */
 /*-----------PROCESOS------------------------------------------------*/
 /*------------------------------------------------------------------- */


 //PROCESS(master_report_ChaRoot, "Proceso master de los msg report-ChangeRoot");
 PROCESS(send_message_report_ChaRoot, "Enviar msg de report-ChangeRoot");
 PROCESS(evaluar_msg_rp, "Evalua si los hijos ya enviaron los msg de report");
 PROCESS(e_LWOE, "Evaluar si ya tengo LWOE propio y de los vecinos");
 PROCESS(evaluar_msg_cr, "Evaluar Mensaje de change_root");


PROCESS_THREAD(evaluar_msg_rp, ev, data)
{
    PROCESS_EXITHANDLER();
    PROCESS_BEGIN();

    //inicializar: Adicionalmente se re-inician en FOUND
    list_init(rp_list);
    memb_init(&rp_mem);

    while(1)
    {
        PROCESS_WAIT_EVENT(); // Wait for any event.
        if(ev == PROCESS_EVENT_CONTINUE)
        {

                static report_list *rp_list_p;
                rp_list_p = list_head(rp_list);
                    printf("TL:%d LLLego report de %d Neigh=%d Weight=%d.%02d Hj=%d flags=%04X\n",
                    list_length(rp_list),
                  rp_list_p->from.u8[0],
                  rp_list_p->rp_msg.neighbor_r.u8[0],
                  (int)(rp_list_p->rp_msg.weight_r / SEQNO_EWMA_UNITY),
                  (int)(((100UL * rp_list_p->rp_msg.weight_r) / SEQNO_EWMA_UNITY) % 100),
                   nd.num_children,
                   nd.flags
                   );

                //Si la lista de reports ya esta completa
                if(list_length(rp_list) >= nd.num_children)//Si el tamano de la lista es = al num de hijos
                {
                    //Saco el nodo con menor peso de la lista
                    printf("Lista de Reports completa \n");

                    //Encuentro el menor de la lista
                    static report_list *rp_str = NULL;
                    static uint32_t lowest_weight;
                    static report_list *lowest_rp = NULL;
                    for(rp_str = list_head(rp_list), lowest_weight = rp_str->rp_msg.weight_r,
                        lowest_rp = rp_str;
                        rp_str != NULL; rp_str = rp_str->next)
                    {
                        if(rp_str->rp_msg.weight_r < lowest_weight)
                        {
                            lowest_weight = rp_str->rp_msg.weight_r;
                            lowest_rp     = rp_str;
                        }
                    }

                    //guardo el menor hijo como el mejor edge
                    linkaddr_copy( &nd.downroute , &lowest_rp->from);
                    linkaddr_copy(&nd.lwoe.children.neighbor, &lowest_rp->rp_msg.neighbor_r );
                    nd.lwoe.children.weight = lowest_rp->rp_msg.weight_r;
                    nd.flags |= CH_LWOE; //Ya encontre el ND_LWOE
                    process_post(&e_LWOE, PROCESS_EVENT_CONTINUE, NULL);

                    printf("El menor de la lista es %d weight=%d.%02d flags=%04X - downroute=%d \n",
                    nd.lwoe.children.neighbor.u8[0],
                    (int)(nd.lwoe.children.weight / SEQNO_EWMA_UNITY),
                    (int)(((100UL * nd.lwoe.children.weight) / SEQNO_EWMA_UNITY) % 100),
                    nd.flags,
                    nd.downroute.u8[0]);

                    //Remuevo (list_remove) todos los elementos de la lista
                    for(rp_list_p = list_head(rp_list); rp_list_p != NULL; rp_list_p = rp_list_p->next)
                    {
                        my_list_remove(rp_list, rp_list_p); //Remove a specific element from a list.
                        memb_free(&rp_mem, rp_list_p);
                    }
                } // si lista >= num_children
        } //IF EV == CONTINUE
    } //END OF WHILE
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
        //static struct etimer et;

        PROCESS_WAIT_EVENT(); // Wait for any event.

        if(ev == e_msg_report)
        {
            static report_msg *rp_msg_d; //rp = report
            rp_msg_d = (report_msg *) data;
            static report_msg rp_msg;

            //Delay 2-4 seconds  //Para que no todos lo manden al tiempo
            /*etimer_set(&et, CLOCK_SECOND * 2 + random_rand() % (CLOCK_SECOND * 2));
            PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));*/

            if(!runicast_is_transmitting(&runicast)) // Si runicast no esta TX, entra
            {
                llenar_report_msg(&rp_msg, &nd.parent, &rp_msg_d->neighbor_r,
                                  rp_msg_d->weight_r);
                packetbuf_copyfrom(&rp_msg, sizeof(rp_msg));
                packetbuf_set_attr(PACKETBUF_ATTR_PACKET_GHS_TYPE_MSG, REPORT);
                runicast_send(&runicast, &rp_msg.destination, MAX_RETRANSMISSIONS);
                printf("Envie report a %d Neigh=%d Weight=%d.%02d flags=%04X\n",
                        rp_msg.destination.u8[0],
                        rp_msg.neighbor_r.u8[0],
                        (int)( rp_msg.weight_r / SEQNO_EWMA_UNITY),
                        (int)(((100UL * rp_msg.weight_r ) / SEQNO_EWMA_UNITY) % 100),
                        nd.flags );
            }else //Si runicast esta ocupado TX, pospongo el envio del msg
            {
                //pospone sending the message
                llenar_report_msg(&rp_msg, &nd.parent, &rp_msg_d->neighbor_r,
                                  rp_msg_d->weight_r);
                process_post(PROCESS_CURRENT(), e_msg_report, &rp_msg);
            }
        }else
        if(ev == e_msg_ch_root)
        {
            static change_root_msg *cr_msg_d; //cr = change root
            cr_msg_d = (change_root_msg *) data;
            static change_root_msg cr_msg;

            //llenar_change_root(&cr_msg, &cr_msg_d->next_hop, &cr_msg_d->final_destination);

            //Delay 2-4 seconds  //Para que no todos lo manden al tiempo
            /*etimer_set(&et, CLOCK_SECOND * 2 + random_rand() % (CLOCK_SECOND * 2));
            PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));*/

            if(!runicast_is_transmitting(&runicast)) // Si runicast no esta TX, entra
            {
                llenar_change_root(&cr_msg, &cr_msg_d->next_hop, &cr_msg_d->final_destination);
                packetbuf_copyfrom(&cr_msg, sizeof(cr_msg));
                packetbuf_set_attr(PACKETBUF_ATTR_PACKET_GHS_TYPE_MSG, CHANGE_ROOT);
                runicast_send(&runicast, &cr_msg.next_hop, MAX_RETRANSMISSIONS);
                printf("Envie CHANGE_RooT next_hop=%d final_destination=%d\n",
                        cr_msg.next_hop.u8[0],
                        cr_msg.final_destination.u8[0]
                        );
            }else //Si runicast esta ocupado TX, pospongo el envio del msg
            {
                //pospone sending the message
                llenar_change_root(&cr_msg, &cr_msg_d->next_hop, &cr_msg_d->final_destination);
                process_post(PROCESS_CURRENT(), e_msg_ch_root, &cr_msg);
            }
        }
    } //end of infinite while
    PROCESS_END();
}

/* Proceso para evaluar si ya tengo LWOE propio (ND_LWOE) y de los vecinos (CH_LWOE)
*/
PROCESS_THREAD(e_LWOE, ev, data)
{
    PROCESS_EXITHANDLER();
    PROCESS_BEGIN();

    while(1)
    {
        static report_msg rp_msg; //rp = report
        static change_root_msg cr_msg; //cr = change root
        static connect_msg co_msg;

        PROCESS_WAIT_EVENT(); // Wait for any event.
        if(ev == PROCESS_EVENT_CONTINUE)
        {
            if(nd.flags & CORE_NODE)
            {
                if( (nd.flags & ND_LWOE) && (nd.flags & CH_LWOE) )
                {
                    if( nd.lwoe.node.weight <= nd.lwoe.children.weight ) //Si es mejor MI edge
                    {
                        //Envio CONNECT msg
                        llenar_connect_msg (&co_msg, nd.f.level, &nd.lwoe.node.neighbor);
                        process_post(&send_message_co_i,  e_msg_connect, &co_msg);
                        printf("Deseo CONNECT a %d\n", nd.lwoe.node.neighbor.u8[0]);
                    }else //Si es mejor el edge de un vecino
                    {
                        //send change_root y dejo de ser CORE_NODE
                        //nd.flags &= ~CORE_NODE;
                        llenar_change_root(&cr_msg, &nd.downroute, &nd.lwoe.children.neighbor);
                        process_post(&send_message_report_ChaRoot, e_msg_ch_root, &cr_msg );
                        printf("EEEnvie 2 CHANGE_ROOT a next_hop=%d final_destination=%d\n",
                        cr_msg.next_hop.u8[0],
                        cr_msg.final_destination.u8[0]);
                    }
                }else //SI NO estan seteados los dos
                {
                    if( (es_Hoja()) && (nd.flags & ND_LWOE) )
                    {
                        llenar_report_msg(&rp_msg, &nd.parent , &linkaddr_node_addr, nd.lwoe.node.weight );
                        process_post(&send_message_report_ChaRoot, e_msg_report , &rp_msg);
                        printf("CORE_NODE & HOJA Deseo Reportar Neigh=%d Weight=%d.%02d\n",
                                 rp_msg.neighbor_r.u8[0],
                                 (int)(rp_msg.weight_r / SEQNO_EWMA_UNITY),
                                 (int)(((100UL * rp_msg.weight_r) / SEQNO_EWMA_UNITY) % 100));
                        //paso a FOUND
                        process_post(&master_co_i, e_found, NULL);
                        nd.state = FOUND;   //Para saber en que estado estoy en cualquier parte

                    }
                }
            }else //NO soy el CORE_NODE
            {
                if( (nd.flags & ND_LWOE) && (nd.flags & CH_LWOE) )
                {
                    if( nd.lwoe.node.weight <= nd.lwoe.children.weight ) //Si es mejor MI edge
                    {
                        //send_report y paso a estado FOUND
                        llenar_report_msg(&rp_msg, &nd.parent , &linkaddr_node_addr, nd.lwoe.node.weight );
                        process_post(&send_message_report_ChaRoot, e_msg_report, &rp_msg);
                        printf("NO_CORE & BEST(ND_LWOE) Deseo Reportar Neigh=%d Weight=%d.%02d\n",
                                 nd.lwoe.node.neighbor.u8[0],
                                 (int)(nd.lwoe.node.weight / SEQNO_EWMA_UNITY),
                                 (int)(((100UL * nd.lwoe.node.weight) / SEQNO_EWMA_UNITY) % 100)  );
                        //paso a FOUND
                        process_post(&master_co_i, e_found, NULL);
                        nd.state = FOUND;   //Para saber en que estado estoy en cualquier parte

                    }else //Si es mejor el edge de un vecino
                    {
                        //send_report y paso a estado FOUND
                        // nd.lwoe.children.neighbor es neighbor_r
                        llenar_report_msg(&rp_msg, &nd.parent , &nd.lwoe.children.neighbor, nd.lwoe.children.weight );
                        process_post(&send_message_report_ChaRoot, e_msg_report, &rp_msg);
                        printf("NO_CORE & BEST(CH_LWOE) Deseo Reportar Neigh=%d Weight=%d.%02d\n",
                                 nd.lwoe.children.neighbor.u8[0],
                                 (int)(nd.lwoe.children.weight / SEQNO_EWMA_UNITY),
                                 (int)(((100UL * nd.lwoe.children.weight) / SEQNO_EWMA_UNITY) % 100)  );
                        //paso a FOUND
                        process_post(&master_co_i, e_found, NULL);
                        nd.state = FOUND;   //Para saber en que estado estoy en cualquier parte

                    }
                }else //SI NO estan seteados los dos
                {
                    if( (es_Hoja()) && (nd.flags & ND_LWOE) )
                    {
                        llenar_report_msg(&rp_msg, &nd.parent , &linkaddr_node_addr, nd.lwoe.node.weight );
                        process_post(&send_message_report_ChaRoot, e_msg_report , &rp_msg);
                        printf("NO_CORE & HOJA Deseo Reportar Neigh=%d Weight=%d.%02d\n",
                                 rp_msg.neighbor_r.u8[0],
                                 (int)(rp_msg.weight_r / SEQNO_EWMA_UNITY),
                                 (int)(((100UL * rp_msg.weight_r) / SEQNO_EWMA_UNITY) % 100));
                         //paso a FOUND
                         process_post(&master_co_i, e_found, NULL);
                         nd.state = FOUND;   //Para saber en que estado estoy en cualquier parte
                    }
                }
            }
        } //IF ev == CONTINUE
    } //end of while

    PROCESS_END();

}

/* Evaluar msg de change root = cr
*/
PROCESS_THREAD(evaluar_msg_cr, ev, data)
{
    PROCESS_BEGIN();

    // Iniciar la lista
    list_init(cr_list);
    memb_init(&cr_mem);

    while(1)
    {
        PROCESS_WAIT_EVENT(); // Wait for any event.
        if(ev == PROCESS_EVENT_CONTINUE)
        {
            if(list_length(cr_list))
            {
                change_root_list *cr_list_p;
                for(cr_list_p = list_head(cr_list); cr_list_p != NULL; cr_list_p = cr_list_p->next)
                {

                    static connect_msg c_msg;
                    static change_root_msg cr_msg_new;

                    //Si el change_root es para mi
                    if(linkaddr_cmp(&cr_list_p->cr_msg.final_destination, &linkaddr_node_addr)) //Entra si las direcciones son iguales
                    {
                        //El msg de CHANGE_ROOT ES PARA MI
                        printf("El msg de ChangeRooot es para mi\n");
                        become_branch(e_list_head_g, &nd.lwoe.node.neighbor); // become branch de change root

                        llenar_connect_msg (&c_msg, nd.f.level, &nd.lwoe.node.neighbor);
                        process_post(&send_message_co_i,  e_msg_connect, &c_msg);
                        printf("Deseo CONNECT a %d\n", nd.lwoe.node.neighbor.u8[0]);

                    }else//Si el change_root NO es para mi
                    {
                        printf("El msg de ChangeRooot NO es para mi\n");

                        llenar_change_root(&cr_msg_new, &nd.downroute, &cr_list_p->cr_msg.final_destination);
                        process_post(&send_message_report_ChaRoot, e_msg_ch_root, &cr_msg_new );
                        printf("REEEnvie  CHANGE_ROOT a next_hop=%d final_destination=%d\n",
                        cr_msg_new.next_hop.u8[0],
                        cr_msg_new.final_destination.u8[0]);
                    }

                    //remuevo el elemento de la lista
                    my_list_remove(cr_list, cr_list_p); //Remove a specific element from a list.
                    memb_free(&cr_mem, cr_list_p);

                } //FOR recorrer toda la lista
            } //Si hay elementos en la lista
        } //END of EV == CONTINUE
    } //END of WHILE

    PROCESS_END();

} //End of PROCESS THREAD
