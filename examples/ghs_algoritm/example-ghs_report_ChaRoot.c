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

 MEMB(info_mem, informacion_list , MAX_TAMANO_LISTA_MSG); // Defines a memory pool for edges
 LIST(info_list); // List that holds the neighbors we have seen thus far


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
        printf("runicast message received from %d.%d, seqno %d (DUPLICATE)\n",
  	     from->u8[0], from->u8[1], seqno);
        return;
      }
      /* Update existing history entry */
      e->seq = seqno;
    }
       //Leer el packet buffer attribute: Especificamente el tipo de mensaje
       packetbuf_attr_t msg_type = packetbuf_attr(PACKETBUF_ATTR_PACKET_GHS_TYPE_MSG);
       if(msg_type == REPORT)
       {
          report_list *rp_list_p;
          rp_list_p = memb_alloc(&rp_mem); //Alocar memoria
          if(rp_list_p == NULL)
          {
              printf("ERROR: La lista de msg de REPORT esta llena\n");
          }else
          {
              rp_list_p->rp_msg = *((report_msg *)msg); //msg le hago cast.Luego cojo todo el msg
              linkaddr_copy(&rp_list_p->from, from);
              list_add(rp_list, rp_list_p); //Add an item at the end of a list.
              process_post(&evaluar_msg_rp, PROCESS_EVENT_CONTINUE, NULL);
              //process_poll(&evaluar_msg_rp);
          }
       }else //end IF REPORT
       if(msg_type == CHANGE_ROOT)
       {
           change_root_list *cr_list_p;
           cr_list_p = memb_alloc(&cr_mem); //Alocar memoria
           if(cr_list_p == NULL)
           {
               printf("ERROR: La lista de msg de change_root esta llena\n");
           }else
           {
               cr_list_p->cr_msg = *((change_root_msg *)msg); //msg le hago cast.Luego cojo todo el msg
               linkaddr_copy(&cr_list_p->from, from);
               list_add(cr_list, cr_list_p); //Add an item at the end of a list.
               process_post(&evaluar_msg_cr, PROCESS_EVENT_CONTINUE, NULL);
               //process_poll(&evaluar_msg_cr);
           }
       }else
       if(msg_type == INFORMATION)
       {
           informacion_list *info_list_p;
           info_list_p = memb_alloc(&info_mem); //Alocar memoria
           if(info_list_p == NULL)
           {
               printf("ERROR: La lista de msg de INFORMACION esta llena\n");
           }else
           {
               //no necesito coger el mensage
               linkaddr_copy(&info_list_p->from, from);
               list_add(info_list, info_list_p); //Add an item at the end of a list.
               process_post(&evaluar_msg_info, PROCESS_EVENT_CONTINUE, NULL);
               //process_poll(&evaluar_msg_info);
           }
       }

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
 PROCESS(evaluar_msg_info, "Evaluar Mensaje de informacion");


PROCESS_THREAD(evaluar_msg_rp, ev, data)
{
    PROCESS_EXITHANDLER();
    PROCESS_BEGIN();

    //inicializar: Adicionalmente se re-inician en FOUND
    list_init(rp_list);
    memb_init(&rp_mem);

    while(1)
    {
        //PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);
        PROCESS_WAIT_EVENT(); // Wait for any event.
        if(ev == PROCESS_EVENT_CONTINUE)
        {

                static report_list *rp_list_p;
                rp_list_p = list_head(rp_list);
                    printf("TL:%d LLLego report de %d Neigh=%d Weight=%d.%02d  flags=%04X\n",
                    list_length(rp_list),
                  rp_list_p->from.u8[0],
                  rp_list_p->rp_msg.neighbor_r.u8[0],
                  (int)(rp_list_p->rp_msg.weight_r / SEQNO_EWMA_UNITY),
                  (int)(((100UL * rp_list_p->rp_msg.weight_r) / SEQNO_EWMA_UNITY) % 100),
                   nd.flags
                   );
                   printf("list_length(rp_list)=%d  \n",
                    list_length(rp_list) );

                //Si la lista esta CASI completa
                if( lista_casi_completa(rp_list) )
                {
                    process_post(&e_LWOE, PROCESS_EVENT_CONTINUE, NULL);
                }else
                //Si la lista de reports ya esta completa
                if(list_length(rp_list) >= num_hijos(e_list_head_g) ) //Si el tamano de la lista es = al num de hijos
                {
                    //Saco el nodo con menor peso de la lista
                    printf("Reports completos..   \n");

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
                } // si lista >= num children
        } //IF EV == CONTINUE
    } //END OF WHILE
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
                        printf("nd.flags=%04X\n", nd.flags);
                        if( (nd.flags & ND_LWOE) && (nd.flags & CH_LWOE) )
                        {
                            printf("Entraaa nd.flags=%04X\n", nd.flags);

                            if( (nd.lwoe.node.weight == INFINITO) && (nd.lwoe.children.weight==INFINITO) )
                            {
                                printf("Los dos reportes son INFINITO\n");
                                //Paso a FOUND
                                process_post(&master_co_i, e_found, NULL);
                                nd.state = FOUND;   //Para saber en que estado estoy en cualquier parte
                                //paso a END
                                process_post(&master_co_i, e_msg_ghs_end, NULL);
                            }else
                            {
                                if(!(nd.flags & FRAGMENTO_LWOE))
                                {
                                    printf("nd.lwoe.node.weigh=%d.%02d <= nd.lwoe.children.weight=%d.%02d \n",
                                    (int)(nd.lwoe.node.weight / SEQNO_EWMA_UNITY),
                                    (int)(((100UL * nd.lwoe.node.weight) / SEQNO_EWMA_UNITY) % 100),
                                    (int)(nd.lwoe.children.weight / SEQNO_EWMA_UNITY),
                                    (int)(((100UL * nd.lwoe.children.weight) / SEQNO_EWMA_UNITY) % 100)     );

                                    if( nd.lwoe.node.weight <= nd.lwoe.children.weight ) //Si es mejor MI edge
                                    {
                                        nd.flags &= ~CORE_NODE;
                                        //envio CHANGE_ROOT imaginario
                                        become_branch(e_list_head_g, &nd.lwoe.node.neighbor); // become branch de change root

                                        //Envio CONNECT msg
                                        llenar_connect_msg (&co_msg, nd.f.level, &nd.lwoe.node.neighbor);
                                        process_post(&send_message_co_i,  e_msg_connect, &co_msg);
                                        printf("Deseo CONNECT a %d\n", nd.lwoe.node.neighbor.u8[0]);
                                        //paso a FOUND
                                        process_post(&master_co_i, e_found, NULL);
                                        nd.state = FOUND;   //Para saber en que estado estoy en cualquier parte

                                    }else //Si es mejor el edge de un vecino
                                    {
                                        //send change_root y dejo de ser CORE_NODE
                                        nd.flags &= ~CORE_NODE;
                                        llenar_change_root(&cr_msg, &nd.downroute, &nd.lwoe.children.neighbor);
                                        process_post(&send_message_report_ChaRoot, e_msg_ch_root, &cr_msg );
                                        printf("EEEnvie 222 CHANGE_ROOT a next_hop=%d final_destination=%d\n",
                                        cr_msg.next_hop.u8[0],
                                        cr_msg.final_destination.u8[0]);
                                        //paso a FOUND
                                        process_post(&master_co_i, e_found, NULL);
                                        nd.state = FOUND;   //Para saber en que estado estoy en cualquier parte
                                    }

                                    //Ya encontre el LWOE del fragmento. No reenvio CHANGE_ROOT
                                    //del otro CORE_NODE
                                    nd.flags |= FRAGMENTO_LWOE;
                                    //Otro CORE_NODE debe dejar de ser CORE_NODE
                                    static informacion_msg inf_msg;
                                    llenar_msg_informacion(&inf_msg, NO_SEA_CORE_NODE, &nd.otro_core_node );
                                    process_post(&send_message_report_ChaRoot, e_msg_information, &inf_msg);
                                }else
                                {
                                    //paso a FOUND
                                    process_post(&master_co_i, e_found, NULL);
                                    nd.state = FOUND;   //Para saber en que estado estoy en cualquier parte
                                }
                            } //SI los dos valores son INFINITO acabo GHS
                        }else //SI NO estan seteados los dos. Pero soy hoja...
                        if( (es_Hoja()) && (nd.flags & ND_LWOE) )
                        {
                            if(nd.lwoe.node.weight == INFINITO)
                            {
                                process_post(&master_co_i, e_msg_ghs_end, NULL);
                            }

                            nd.flags &= ~CORE_NODE;
                            llenar_report_msg(&rp_msg, &nd.parent , &linkaddr_node_addr, nd.lwoe.node.weight );
                            process_post(&send_message_report_ChaRoot, e_msg_report , &rp_msg);
                            printf("Hoja:CORE_NODE & HOJA Deseo Reportar Neigh=%d Weight=%d.%02d\n",
                                     rp_msg.neighbor_r.u8[0],
                                     (int)(rp_msg.weight_r / SEQNO_EWMA_UNITY),
                                     (int)(((100UL * rp_msg.weight_r) / SEQNO_EWMA_UNITY) % 100));
                            //paso a FOUND
                            process_post(&master_co_i, e_found, NULL);
                            nd.state = FOUND;   //Para saber en que estado estoy en cualquier parte
                        }
                        else //SI NO estan seteados los dos. Pero soy lista CASI completa(solo falta el otro core_node)
                        if( (lista_casi_completa(rp_list)) && (nd.flags & ND_LWOE) )
                        {  //No puedo pasar a FOUND ni ~CORE_NODE porque tengo q esperar
                            //a que al menos 1 de los 2 CORE_NODES envie change_root.
                            //Lo que pasa es que los 2 CORE_NODE envian report y pasan a FOUND
                            //y ninguno envia change_root.
                            if(list_length(rp_list) == 0) //Mi unico hijo es el otro CORE_NODE
                            {
                                if(nd.lwoe.node.weight == INFINITO)
                                {
                                    process_post(&master_co_i, e_msg_ghs_end, NULL);
                                }

                                //nd.flags &= ~CORE_NODE;
                                llenar_report_msg(&rp_msg, &nd.parent , &linkaddr_node_addr, nd.lwoe.node.weight );
                                process_post(&send_message_report_ChaRoot, e_msg_report , &rp_msg);
                                printf("CasiCompleta:CORE_NODE & FALTA el otro core_node Deseo Reportar Neigh=%d Weight=%d.%02d\n",
                                         rp_msg.neighbor_r.u8[0],
                                         (int)(rp_msg.weight_r / SEQNO_EWMA_UNITY),
                                         (int)(((100UL * rp_msg.weight_r) / SEQNO_EWMA_UNITY) % 100));

                                //paso a FOUND
                                //process_post(&master_co_i, e_found, NULL);
                                //nd.state = FOUND;   //Para saber en que estado estoy en cualquier parte

                            }else //Si la lista tiene reportes, tengo HIJOS
                            {
                                //report_list *rp_list_p;
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
                                //nd.flags |= CH_LWOE; //Ya encontre el ND_LWOE
                                //process_post(PROCESS_CURRENT(), PROCESS_EVENT_CONTINUE, NULL);

                                printf("CasiCompleta:El menor de la lista es %d weight=%d.%02d flags=%04X - downroute=%d \n",
                                nd.lwoe.children.neighbor.u8[0],
                                (int)(nd.lwoe.children.weight / SEQNO_EWMA_UNITY),
                                (int)(((100UL * nd.lwoe.children.weight) / SEQNO_EWMA_UNITY) % 100),
                                nd.flags,
                                nd.downroute.u8[0]);

                                //Remuevo (list_remove) todos los elementos de la lista
                                /*for(rp_list_p = list_head(rp_list); rp_list_p != NULL; rp_list_p = rp_list_p->next)
                                {
                                    my_list_remove(rp_list, rp_list_p); //Remove a specific element from a list.
                                    memb_free(&rp_mem, rp_list_p);
                                }*/

                                if( nd.lwoe.node.weight <= nd.lwoe.children.weight ) //Si es mejor MI edge
                                {
                                    if(nd.lwoe.node.weight == INFINITO)
                                    {
                                        process_post(&master_co_i, e_msg_ghs_end, NULL);
                                    }

                                    //nd.flags &= ~CORE_NODE;
                                    llenar_report_msg(&rp_msg, &nd.parent , &linkaddr_node_addr, nd.lwoe.node.weight );
                                    process_post(&send_message_report_ChaRoot, e_msg_report , &rp_msg);
                                    printf("CORE_NODE & FALTA el otro core_node Deseo Reportar Neigh=%d Weight=%d.%02d\n",
                                             rp_msg.neighbor_r.u8[0],
                                             (int)(rp_msg.weight_r / SEQNO_EWMA_UNITY),
                                             (int)(((100UL * rp_msg.weight_r) / SEQNO_EWMA_UNITY) % 100));

                                    //paso a FOUND
                                    //process_post(&master_co_i, e_found, NULL);
                                    //nd.state = FOUND;   //Para saber en que estado estoy en cualquier parte
                               }else
                               {
                                   if(nd.lwoe.children.weight == INFINITO)
                                   {
                                       process_post(&master_co_i, e_msg_ghs_end, NULL);
                                   }

                                   //nd.flags &= ~CORE_NODE;
                                   llenar_report_msg(&rp_msg, &nd.parent , &nd.lwoe.children.neighbor, nd.lwoe.children.weight );
                                   process_post(&send_message_report_ChaRoot, e_msg_report, &rp_msg);
                                   printf("CORE_NODE & FALTA el otro core_node Deseo Reportar Neigh=%d Weight=%d.%02d\n",
                                                        rp_msg.neighbor_r.u8[0],
                                                        (int)(rp_msg.weight_r / SEQNO_EWMA_UNITY),
                                                        (int)(((100UL * rp_msg.weight_r) / SEQNO_EWMA_UNITY) % 100));
                                   //paso a FOUND
                                   //process_post(&master_co_i, e_found, NULL);
                                   //nd.state = FOUND;   //Para saber en que estado estoy en cualquier parte

                               }
                            }
                        }
                }else //NO soy el CORE_NODE
                {
                    if( (nd.flags & ND_LWOE) && (nd.flags & CH_LWOE) )
                    {
                        if( nd.lwoe.node.weight <= nd.lwoe.children.weight ) //Si es mejor MI edge
                        {
                            if(nd.lwoe.node.weight == INFINITO)
                            {
                                process_post(&master_co_i, e_msg_ghs_end, NULL);
                            }
                            //send_report y paso a estado FOUND
                            llenar_report_msg(&rp_msg, &nd.parent , &linkaddr_node_addr, nd.lwoe.node.weight );
                            process_post(&send_message_report_ChaRoot, e_msg_report, &rp_msg);
                            printf("NO_CORE & BEST(ND_LWOE) Deseo Reportar Neigh=%d Weight=%d.%02d\n",
                                                rp_msg.neighbor_r.u8[0],
                                                (int)(rp_msg.weight_r / SEQNO_EWMA_UNITY),
                                                (int)(((100UL * rp_msg.weight_r) / SEQNO_EWMA_UNITY) % 100));
                            //paso a FOUND
                            process_post(&master_co_i, e_found, NULL);
                            nd.state = FOUND;   //Para saber en que estado estoy en cualquier parte

                        }else //Si es mejor el edge de un vecino
                        {
                            if(nd.lwoe.children.weight == INFINITO)
                            {
                                process_post(&master_co_i, e_msg_ghs_end, NULL);
                            }
                            //send_report y paso a estado FOUND
                            // nd.lwoe.children.neighbor es neighbor_r
                            llenar_report_msg(&rp_msg, &nd.parent , &nd.lwoe.children.neighbor, nd.lwoe.children.weight );
                            process_post(&send_message_report_ChaRoot, e_msg_report, &rp_msg);
                            printf("NO_CORE & BEST(CH_LWOE) Deseo Reportar Neigh=%d Weight=%d.%02d\n",
                                                rp_msg.neighbor_r.u8[0],
                                                (int)(rp_msg.weight_r / SEQNO_EWMA_UNITY),
                                                (int)(((100UL * rp_msg.weight_r) / SEQNO_EWMA_UNITY) % 100));
                            //paso a FOUND
                            process_post(&master_co_i, e_found, NULL);
                            nd.state = FOUND;   //Para saber en que estado estoy en cualquier parte

                        }
                    }else //SI NO estan seteados los dos
                    if( (es_Hoja()) && (nd.flags & ND_LWOE) )
                    {
                        if(nd.lwoe.node.weight == INFINITO)
                        {
                            process_post(&master_co_i, e_msg_ghs_end, NULL);
                        }
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
                } //END no soy core node
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
        //PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);
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
                        printf("El msg de ChangeRooot es para mi, from=%d\n",
                        cr_list_p->from.u8[0]);

                        become_branch(e_list_head_g, &nd.lwoe.node.neighbor); // become branch de change root

                        //Envio CONNECT
                        llenar_connect_msg (&c_msg, nd.f.level, &nd.lwoe.node.neighbor);
                        process_post(&send_message_co_i,  e_msg_connect, &c_msg);
                        printf("Deseo CONNECT a %d\n", nd.lwoe.node.neighbor.u8[0]);

                    }else//Si el change_root NO es para mi
                    {

                        //si voy a reenviar un CHANGE_ROOT que viene del otro_core_node &&
                        //Ya encontre el LWOE del fragmento
                        if(  (linkaddr_cmp(&cr_list_p->from,&nd.otro_core_node)) &&
                             (nd.flags & FRAGMENTO_LWOE) )
                        {
                            printf("ChangeRoot NO es para mi - NO Reenvio el CHANGE_ROOT porque YO ya lo mande\n");
                        }else
                        {
                            nd.flags |= FRAGMENTO_LWOE;
                            printf("ChangeRoot NO es para mi\n");
                            llenar_change_root(&cr_msg_new, &nd.downroute, &cr_list_p->cr_msg.final_destination);
                            process_post(&send_message_report_ChaRoot, e_msg_ch_root, &cr_msg_new );
                            printf("REEEnvie  CHANGE_ROOT a next_hop=%d final_destination=%d\n",
                                    cr_msg_new.next_hop.u8[0],
                                    cr_msg_new.final_destination.u8[0]);
                        }
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


PROCESS_THREAD(evaluar_msg_info, ev, data)
{
    PROCESS_BEGIN();

    // Iniciar la lista
    list_init(info_list);
    memb_init(&info_mem);

    static report_list *rp_list_p;
    static informacion_list *info_list_p;

    while(1)
    {
        //PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);
        PROCESS_WAIT_EVENT(); // Wait for any event.
        if(ev == PROCESS_EVENT_CONTINUE)
        {
            if(list_length(info_list))
            {
                for(info_list_p = list_head(info_list); info_list_p != NULL; info_list_p = info_list_p->next)
                {
                    printf("llego informacion de =%d\n", info_list_p->from.u8[0]);

                    //dejo de ser core_node
                    nd.flags &= ~CORE_NODE;

                    //limpio mi lista de reportes
                    for(rp_list_p = list_head(rp_list); rp_list_p != NULL; rp_list_p = rp_list_p->next)
                    {
                        my_list_remove(rp_list, rp_list_p); //Remove a specific element from a list.
                        memb_free(&rp_mem, rp_list_p);
                    }

                    //paso a FOUND
                    process_post(&master_co_i, e_found, NULL);
                    nd.state = FOUND;   //Para saber en que estado estoy en cualquier parte

                    //remuevo el elemento de la lista
                    my_list_remove(info_list, info_list_p); //Remove a specific element from a list.
                    memb_free(&info_mem, info_list_p);

                } //end FOR
            }// END IF lista tiene algun elemento
        } //IF ev==CONTINUE
    } //end WHILE

    PROCESS_END();
}

PROCESS_THREAD(send_message_report_ChaRoot, ev, data)
{
    PROCESS_EXITHANDLER();
    PROCESS_BEGIN();

    //proceso report - ChangeRoot
    e_msg_report          = process_alloc_event(); // Darle un numero al evento
    e_msg_ch_root         = process_alloc_event(); // Darle un numero al evento
    e_msg_information     = process_alloc_event(); // Darle un numero al evento
    e_msg_ghs_end         = process_alloc_event(); // Darle un numero al evento

    process_start(&evaluar_msg_rp, NULL); //para inicializar report_list_g y report_memb_g
    process_start(&e_LWOE, NULL); //para inicializar report_list_g y report_memb_g
    process_start(&evaluar_msg_cr, NULL); //para inicializar report_list_g y report_memb_g
    process_start(&evaluar_msg_info, NULL); //init msg evaluation

    runicast_open(&runicast, 146, &runicast_callbacks); //el 144-145 ya estan usados

    /* OPTIONAL: Sender history */
    list_init(history_list);
    memb_init(&history_mem);

    static report_msg *rp_msg_d; //rp = report
    static report_msg rp_msg;

    static change_root_msg *cr_msg_d; //cr = change root
    static change_root_msg cr_msg;

    static informacion_msg *inf_msg_d; //information_message
    static informacion_msg inf_msg;

    while(1)
    {
        PROCESS_WAIT_EVENT(); // Wait for any event.

        if(ev == e_msg_report)
        {
            rp_msg_d = (report_msg *) data;

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
            cr_msg_d = (change_root_msg *) data;

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
        }else
        if(ev == e_msg_information)
        {
            inf_msg_d = (informacion_msg *) data;

            if(!runicast_is_transmitting(&runicast)) // Si runicast no esta TX, entra
            {
                llenar_msg_informacion(&inf_msg, inf_msg_d->flags, &inf_msg_d->destination);
                packetbuf_copyfrom(&inf_msg, sizeof(inf_msg));
                packetbuf_set_attr(PACKETBUF_ATTR_PACKET_GHS_TYPE_MSG, INFORMATION);
                runicast_send(&runicast, &inf_msg.destination, MAX_RETRANSMISSIONS);
                printf("Envie INFORMATION msg TO=%d\n",
                        inf_msg.destination.u8[0]);
            }else //Si runicast esta ocupado TX, pospongo el envio del msg
            {
                //pospone sending the message
                llenar_msg_informacion(&inf_msg, inf_msg_d->flags, &inf_msg_d->destination);
                process_post(PROCESS_CURRENT(), e_msg_information, &inf_msg);
            }
        }

    } //end of infinite while
    PROCESS_END();
}
