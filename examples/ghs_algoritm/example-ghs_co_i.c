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

MEMB(edges_memb, edges, MAX_NEIGHBORS); // Defines a memory pool for edges
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

// Lista para guardar los msg de connect de salida (out)
MEMB(co_mem_out, connect_list , MAX_TAMANO_LISTA_MSG);
LIST(co_list_out);

list_t co_list_out_g;
struct memb *co_mem_out_g;

// Lista para guardar los msg de initiate
MEMB(i_mem,  initiate_list   , MAX_TAMANO_LISTA_MSG);
LIST(i_list);

// Lista para guardar los msg de initiate
MEMB(i_mem_out,  initiate_list   , MAX_TAMANO_LISTA_MSG);
LIST(i_list_out);

list_t i_list_out_g;
struct memb *i_mem_out_g;


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

   // OPTIONAL: Sender history
   void *msg = packetbuf_dataptr();
   struct history_entry *e = NULL;

   for(e = list_head(history_list); e != NULL; e = e->next) {
     if(linkaddr_cmp(&e->addr, from)) { // Si las dir son iguales entra
       break;
     }
   }
   if(e == NULL) {
     // Create new history entry
     e = memb_alloc(&history_mem);
     if(e == NULL) {
       e = list_chop(history_list); /* Remove oldest at full history */
     }
     linkaddr_copy(&e->addr, from);
     e->seq = seqno;
     list_push(history_list, e);
   } else {
     // Detect duplicate callback
     if(e->seq == seqno) {
       MY_DBG_1("runicast (co-i) message received from %d.%d, seqno %d (DUPLICATE)\n",
        from->u8[0], from->u8[1], seqno);
       return;
     }
     // Update existing history entry
     e->seq = seqno;
   }

   //Leer el packet buffer attribute: Especificamente el tipo de mensaje
   packetbuf_attr_t msg_type = packetbuf_attr(PACKETBUF_ATTR_PACKET_GHS_TYPE_MSG);

   // Evaluo el tipo de msg que llego
   if(msg_type == CONNECT)
   {
       connect_list *co_list_p;
       co_list_p = memb_alloc(&co_mem); //Alocar memoria
       if(co_list_p == NULL)
       {
           MY_DBG_1("ERROR: La lista de msg de connect esta llena\n");
       }else
       {
           co_list_p->co_msg = *((connect_msg *)msg); //msg le hago cast.Luego cojo todo el msg
           linkaddr_copy(&co_list_p->from, from);
           list_add(co_list, co_list_p); //Add an item at the end of a list.
           MY_DBG_3("llego connect de from %d\n",from->u8[0] );
           //process_post_synch(&evaluar_msg_co, PROCESS_EVENT_CONTINUE, NULL);
           process_post(&evaluar_msg_co, PROCESS_EVENT_CONTINUE, NULL);

       }

   }else
   if(msg_type == INITIATE)
   {

       initiate_list *i_list_p;
       i_list_p = memb_alloc(&i_mem); //Alocar memoria
       if(i_list_p == NULL)
       {
          MY_DBG_1("ERROR: La lista de msg de initiate esta llena\n");
       }else
       {
          i_list_p->i_msg = *((initiate_msg *)msg); //msg le hago cast.Luego cojo todo el msg
          linkaddr_copy(&i_list_p->from, from);
          list_add(i_list, i_list_p); //Add an item at the end of a list.
          //process_post_synch(&evaluar_msg_i, PROCESS_EVENT_CONTINUE, NULL);
          process_post(&evaluar_msg_i, PROCESS_EVENT_CONTINUE, NULL);

          //LLamar al proceso para que evalue el pospone agregado o actualizado
          // Se hace aca porque el INITIATE es quien cambia el level del fragmento
          process_post(&evaluar_msg_co, PROCESS_EVENT_CONTINUE, NULL); //"fragment is or becomes greater than l" pg. 83 del pdf
          process_post(&evaluar_msg_test, PROCESS_EVENT_CONTINUE, NULL ) ;
       }

   }else{
      MY_DBG_1("ERROR: No se de que tipo es CONNECT o INITIATE");
   } //END if msg es INITIATE

}
static void sent_runicast(struct runicast_conn *c, const linkaddr_t *to, uint8_t retransmissions)
{
  MY_DBG_3("runicast (co-i) message sent to %d.%d, retransmissions %d\n",
     to->u8[0], to->u8[1], retransmissions);
}
static void
timedout_runicast(struct runicast_conn *c, const linkaddr_t *to, uint8_t retransmissions)
{
    MY_DBG_1("ERROR: runicast (co-i) message timed out when sending to %d.%d, retransmissions %d\n",
     to->u8[0], to->u8[1], retransmissions);
}
static const struct runicast_callbacks runicast_callbacks = {recv_runicast,
							     sent_runicast,
							     timedout_runicast};

/*------------------------------------------------------------------- */
/*----------PROCESSES------- -----------------------------------------*/
/*------------------------------------------------------------------- */

PROCESS(master_co_i, "Proceso master de los msg connect-initiate");
PROCESS(send_message_co_i, "Enviar msg de connect - initiate");
PROCESS(evaluar_msg_co, "Evaluar Mensaje de Connect");
PROCESS(evaluar_msg_i, "Evaluar Mensaje de initiate");
PROCESS(interface_GHS_and_Self_healing, "Interface entre el algoritmo GHS y self-healing");
/*------------------------------------------------------------------- */
/*-----------PROCESOS------------------------------------------------*/
/*------------------------------------------------------------------- */


PROCESS_THREAD(interface_GHS_and_Self_healing, ev, data)
{

    PROCESS_BEGIN();

    char message[13];
    char *filename = "msg_file";
    int fd_write, n;//, fd_read;
    edges *e_aux = NULL; // Apuntador que apunta a la tabla de edges

    while(1)
    {
        PROCESS_WAIT_EVENT(); // Wait for any event.
        if(ev == PROCESS_EVENT_CONTINUE)
        {
            for(e_aux = e_list_head_g; e_aux != NULL; e_aux = e_aux->next) // Recorrer toda la lista
            {
                if(linkaddr_cmp(&e_aux->addr, &nd.parent)) //Si es padra envio un 1
                {
                    sprintf(message, "%d %d %d %d.%02d %d\n\r",//va con pri ntf porque siempre quiero ver esto
                    linkaddr_node_addr.u8[0],
                    e_aux->addr.u8[0],
                    1, //El 1 indica que es PADRE
                    (int)(e_aux->weight / SEQNO_EWMA_UNITY), (int)(((100UL * e_aux->weight) / SEQNO_EWMA_UNITY) % 100),
                    e_aux->state);
                }else // Si no es padre
                {
                    sprintf(message, "%d %d %d %d.%02d %d\n\r",//va con printf porque siempre quiero ver esto
                    linkaddr_node_addr.u8[0],
                    e_aux->addr.u8[0],
                    0, //El 1 indica que es PADRE
                    (int)(e_aux->weight / SEQNO_EWMA_UNITY), (int)(((100UL * e_aux->weight) / SEQNO_EWMA_UNITY) % 100),
                    e_aux->state);
                }
                // writing to cfs
                fd_write = cfs_open(filename, CFS_WRITE | CFS_APPEND);
                if(fd_write != -1) //El archivo se abre correctamente
                {

                          n = cfs_write(fd_write, message, sizeof(message));
                          if(n != -1) //El archivo se escribe correctamente
                          {
                              cfs_close(fd_write);
                              MY_DBG_3("step 2: successfully written to cfs. wrote %i bytes\n", n);
                          }
                          else
                          {
                            MY_DBG_1("ERROR: No se pudo abrir el archivo %s para Escritura \n", filename);
                          }
                } else
                {
                  MY_DBG_1("ERROR: No se pudo abrir el archivo %s para Escritura \n", filename);
                }

            } //END FOR recorre la lista
        } //END IF continue
    } //END WHILE
    PROCESS_END();
} //END PROCESS_THREAD

/* Proceso master que controla el find y el found
*/
PROCESS_THREAD(master_co_i, ev, data)
{

    PROCESS_BEGIN();

    /* OPTIONAL: Sender history */
    list_init(edges_list);
    memb_init(&edges_memb);

    static connect_list *co_list_out_p = NULL;
    static s_wait str_wait;
    //static report_list *rp_list_p;

    while(1)
    {
        PROCESS_WAIT_EVENT(); // Wait for any event.
        if (ev == e_init_master_co_i)
        {
            //Terminar procesos
            process_exit(&master_neighbor_discovery);   //Se cierra el proceso y se llama el PROCESS_EXITHANDLER(funcion)
            //Iniciar procesos co_i
            process_start(&send_message_co_i, NULL);
            //Procesos de test ar
            process_start(&send_message_test_ar, NULL);
            //procesos de report-ChangeRoot
            process_start(&send_message_report_ChaRoot, NULL); //para inicializar report_list_g y report_memb_g

            //Inicializar el master_co_i
            init_master_co_i(data, &edges_memb, edges_list);

            //Espero a que todos hayan inicializado la conexion del connect antes de seguir
            //Ademas, SI NO ESPERO LA LISTA SE IMPRIME MAL: RARO X 2
            llenar_wait_struct(&str_wait, WAIT_NETWORK_STABILIZATION, PROCESS_CURRENT()  );
            process_post(&wait, PROCESS_EVENT_CONTINUE, &str_wait);
            PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);

            //Envio Connect INICIAL con level = 0
            co_list_out_p = memb_alloc(&co_mem_out); //Alocar memoria
            if(co_list_out_p == NULL)
            {
                MY_DBG_1("ERROR: no hay memoria para connect inicial\n");
            }else
            {
                llenar_connect_msg_list (co_list_out_p, nd.f.level, &nd.lwoe.node.neighbor);
                list_add(co_list_out, co_list_out_p); //Add an item at the end of a list
            }

            process_post(&send_message_co_i,  e_msg_connect, NULL);
            MY_DBG_3("Deseo enviar connect INICIAL a %d\n", nd.lwoe.node.neighbor.u8[0]);

        }else
        if (ev == e_found)
        {
            //Espero instrucciones de change_root o initiate
            MY_DBG_3("Estoy en FOUND \n");

            /////////////REINICIAR VARIABLE/////////////////
            //Solamente necesito reiniciar estas 2 variables. NINGUNA MAS. ya hice analisis
            nd.flags &= ~ND_LWOE;
            nd.flags &= ~CH_LWOE;

            //imprimo END
            print_final_result();

            //se queda esperando por instrucciones

        }else
        if (ev == e_find)
        {
            MY_DBG_3("Estoy en FIND \n");
            process_post(&e_test, PROCESS_EVENT_CONTINUE, NULL);
        }else
        if(ev == e_msg_ghs_end)
        {
            print_final_result();
        }
    }//END OF WHILE
    PROCESS_END();
}

PROCESS_THREAD(evaluar_msg_co, ev, data)
{
    PROCESS_BEGIN();

    // Iniciar la lista
    list_init(co_list);
    memb_init(&co_mem);

    static initiate_list *i_list_out_p = NULL;
    static connect_list *co_list_p;
    static name name_str;

    while(1)
    {
        PROCESS_WAIT_EVENT(); // Wait for any event.
        if(ev == PROCESS_EVENT_CONTINUE)
        {
            if(list_length(co_list))
            {
                for(co_list_p = list_head(co_list); co_list_p != NULL; co_list_p = co_list_p->next)
                {
                    if(co_list_p->co_msg.level == nd.f.level) //Si los dos fragmentos tienen el mismo nivel
                    {
                        if(state_is_branch(&co_list_p->from, e_list_head_g)) // Caso inicial. Fragmentos con 1 nodo
                        {
                            MY_DBG_3("Tamano lista=%d Evaluo ConNect de %d con Mismo level es BRANCH, level=%d \n",
                            list_length(co_list),
                             co_list_p->from.u8[0],
                             co_list_p->co_msg.level);

                             become_core_node(&co_list_p->from);

                            //send initiate
                            llenar_name_str(&name_str,weight_with_edge(&co_list_p->from, e_list_head_g),
                                            &co_list_p->from);
                            i_list_out_p = memb_alloc(&i_mem_out); //Alocar memoria
                            if(i_list_out_p == NULL)
                            {
                                MY_DBG_1("ERROR: no hay memoria para initiate\n");
                            }else
                            {
                                llenar_initiate_msg_list(i_list_out_p, name_str ,
                                                    (nd.f.level+1), FIND, &co_list_p->from, BECOME_CORE_NODE);
                                list_add(i_list_out, i_list_out_p); //Add an item at the end of a list
                            }
                            process_post(&send_message_co_i, e_msg_initiate, NULL);

                            //remuevo el elemento de la lista
                            my_list_remove(co_list, co_list_p); //Remove a specific element from a list.
                            memb_free(&co_mem, co_list_p);

                        }else //Si el estado NO es branch (El proceso postpones processing CONECT)
                        {
                            MY_DBG_3("Tamano lista=%d Pospone ConNect... nd.f.level=%d \n",
                             list_length(co_list),
                              nd.f.level);
                        } //FIN de pospone connect
                    }else
                    if(co_list_p->co_msg.level < nd.f.level)
                    {
                        MY_DBG_3("MAYOR. Tamano lista=%d EvaluOO ConNect de %d con level=%d, my level=%d \n",
                        list_length(co_list),
                         co_list_p->from.u8[0],
                         co_list_p->co_msg.level, nd.f.level );

                        become_branch(e_list_head_g, &co_list_p->from); // become branch de connect

                        //PAra que el otro_core_node no deje de serlo
                        //PENDIENTE este if
                        if(linkaddr_cmp(&co_list_p->from, &nd.otro_core_node)) //Entra si las direcciones son iguales
                        {
                            //Send initiate
                            i_list_out_p = memb_alloc(&i_mem_out); //Alocar memoria
                            if( i_list_out_p == NULL)
                            {
                                MY_DBG_1("ERROR: no hay memoria para initiate\n");
                            }else
                            {
                                llenar_initiate_msg_list(i_list_out_p, nd.f.name_str, nd.f.level, nd.state, &co_list_p->from, BECOME_CORE_NODE);
                                list_add(i_list_out, i_list_out_p); //Add an item at the end of a list
                            }
                        }else
                        {
                            //Send initiate
                            i_list_out_p = memb_alloc(&i_mem_out); //Alocar memoria
                            if( i_list_out_p == NULL)
                            {
                                MY_DBG_1("ERROR: no hay memoria para initiate\n");
                            }else
                            {
                                llenar_initiate_msg_list(i_list_out_p, nd.f.name_str, nd.f.level, nd.state, &co_list_p->from, STOP_BEING_CORE_NODE);
                                list_add(i_list_out, i_list_out_p); //Add an item at the end of a list
                            }
                        }

                        process_post(&send_message_co_i, e_msg_initiate, NULL);

                        //remuevo el elemento de la lista
                        my_list_remove(co_list, co_list_p); //Remove a specific element from a list.
                        memb_free(&co_mem, co_list_p);

                    }else // si nodo tiene menor nivel. Segun el libro no es posible: saco error!
                    if(co_list_p->co_msg.level > nd.f.level)
                    {
                         MY_DBG_1("ERROR: Nunca deberia entrar aca porque 'level of qs fragment is at least l' \n");
                    } //FIN de pospone connect

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

    static initiate_list *i_list_out_p = NULL;
    static initiate_list *i_list_p;
    static edges *e_aux;
    static report_list *rp_list_p;

    while(1)
    {
        PROCESS_WAIT_EVENT(); // Wait for any event.
        if(ev == PROCESS_EVENT_CONTINUE)
        {
            if(list_length(i_list))
            {
                for(i_list_p = list_head(i_list); i_list_p != NULL; i_list_p = i_list_p->next)
                {

                    ////////////////////////////////////////////////
                    /////////////GUARDAR INITIATE Y REENVIAR////////
                    ////////////////////////////////////////////////

                    nd.f.name_str  = i_list_p->i_msg.f.name_str;
                    nd.f.level     = i_list_p->i_msg.f.level;
                    nd.state       = i_list_p->i_msg.nd_state;
                    linkaddr_copy(&nd.parent , &i_list_p->from);

                    //Reenvio el msg por todas las BRANCHES
                    for(e_aux = e_list_head_g; e_aux != NULL; e_aux = e_aux->next) // Recorrer toda la lista
                    {
                        //Propagar el INITIATE por las otras ramas
                        //Si es una BRANCH y no es el nodo que me envio el INITIATE (No le devuelvo el msg) (No INITIATE al padre)
                        if( (e_aux->state == BRANCH) && !linkaddr_cmp(&e_aux->addr, &i_list_p->from))
                        {

                            //Send initiate
                            i_list_out_p = memb_alloc(&i_mem_out); //Alocar memoria
                            if(i_list_out_p == NULL)
                            {
                                MY_DBG_1("ERROR: no hay memoria para reEnviar initiate\n");
                            }else
                            {
                                llenar_initiate_msg_list(i_list_out_p, i_list_p->i_msg.f.name_str, i_list_p->i_msg.f.level,
                                                   i_list_p->i_msg.nd_state, &e_aux->addr,
                                                   STOP_BEING_CORE_NODE );
                                list_add(i_list_out, i_list_out_p); //Add an item at the end of a list
                            }
                        }
                    }
                    // envio el post aca para no enviarlo multiples veces dentro del for anterior
                    process_post(&send_message_co_i, e_msg_initiate, NULL);

                    ////////////////////////////////////////////////
                    /////////////REINICIAR VARIABLE/////////////////
                    ////////////////////////////////////////////////

                    nd.flags = 0;
                    nd.lwoe.node.weight = INFINITO;
                    linkaddr_copy(&nd.lwoe.node.neighbor, &linkaddr_node_addr); //si peso es infinito, yo soy vecino
                    nd.lwoe.children.weight = INFINITO;
                    linkaddr_copy(&nd.lwoe.children.neighbor, &linkaddr_node_addr); //si peso es infinito, yo soy vecino
                    linkaddr_copy(&nd.downroute, &linkaddr_node_addr); //yo mismo soy downroute
                    linkaddr_copy(&nd.otro_core_node, &linkaddr_node_addr); //otro core node soy YO
                    //limpio mi lista de reportes
                    for(rp_list_p = list_head(rp_list_g); rp_list_p != NULL; rp_list_p = rp_list_p->next)
                    {
                        my_list_remove(rp_list_g, rp_list_p); //Remove a specific element from a list.
                        memb_free(rp_mem_g, rp_list_p);
                    }

                    ////////////////////////////////////////////////
                    /////////////ACCIONES INITIATE//////////////////
                    ////////////////////////////////////////////////


                    if(i_list_p->i_msg.flags & BECOME_CORE_NODE)
                    {
                        become_core_node(&i_list_p->from);
                    }else
                    if(i_list_p->i_msg.flags & STOP_BEING_CORE_NODE)//PAra que el nodo deje de ser core_node
                    {
                        stop_being_core_node();
                    }

                    if(i_list_p->i_msg.nd_state == FIND) //si cambio de estado a FIND
                    {
                        //Envio un mensaje al master_co_i de find
                        MY_DBG_3("Deseo postear FIND\n");
                        process_post(&master_co_i,  e_find, NULL);
                    }else
                    if(i_list_p->i_msg.nd_state == FOUND) //si cambio de estado a FOUND
                    {
                        MY_DBG_3("Deseo postear FOUND\n");
                        process_post(&master_co_i,  e_found, NULL);
                    }

                    MY_DBG_3("TamanoLista =%d llego INITIATE from %d.%d name=%d.%02d level=%d nd.state=%d parent=%d  i_msg.flags=%04X\n",
                          list_length(i_list),
                          i_list_p->from.u8[0], i_list_p->from.u8[1],
                          (int)(nd.f.name_str.weight / SEQNO_EWMA_UNITY),
                          (int)(((100UL * nd.f.name_str.weight) / SEQNO_EWMA_UNITY) % 100),
                          nd.f.level,
                          nd.state,
                          nd.parent.u8[0],
                          i_list_p->i_msg.flags);

                  //Remuevo el elemento de la lista
                  my_list_remove(i_list, i_list_p); //Remove a specific element from a list.
                  memb_free(&i_mem, i_list_p);

                } //FOR todos los elementos de la lista
            } //Si hay elementos en la lista

            print_final_result(); //Lo ultimo que hago es imprimir resultados
            //Para guardar en el archivo del contiki file system el archivo con la tabla de vecinos y el padre
            process_post(&interface_GHS_and_Self_healing, PROCESS_EVENT_CONTINUE, NULL);
            //Toma cambios ok.....ok.....ok Este ES! x 2 !! x 3 !!!
        } //END IF  EV == CONTINUE
    } //end of while
    PROCESS_END();
} //END of PROCESS THREAD evaluar_msg_i


//BOrrar este comentario. lol

/* Proceso para enviar mensajes
*/
PROCESS_THREAD(send_message_co_i, ev, data)
{
    PROCESS_BEGIN();

    process_start(&evaluar_msg_co, NULL );
    process_start(&evaluar_msg_i, NULL );
    process_start(&interface_GHS_and_Self_healing, NULL);

    runicast_open(&runicast, 144, &runicast_callbacks);

    //Proceso co_i
    //estados
    e_found = process_alloc_event(); // Darle un numero al evento
    e_find = process_alloc_event(); // Darle un numero al evento
    //msg
    e_msg_connect = process_alloc_event(); // Darle un numero al evento
    e_msg_initiate = process_alloc_event();  // Darle un numero al evento

    list_init(history_list);
    memb_init(&history_mem);

    list_init(co_list_out);
    memb_init(&co_mem_out);

    list_init(i_list_out);
    memb_init(&i_mem_out);

    co_list_out_g = co_list_out;
    co_mem_out_g  = &co_mem_out;

    i_list_out_g = i_list_out;
    i_mem_out_g  = &i_mem_out;

    static connect_msg co_msg;
    static initiate_msg  i_msg;
    static connect_list *co_list_out_p;
    static initiate_list *i_list_out_p;

    while(1)
    {
        PROCESS_WAIT_EVENT(); // Wait for any event.
        if(ev == e_msg_connect)
        {
            if(list_length(co_list_out))
            {
                for(co_list_out_p = list_head(co_list_out); co_list_out_p != NULL; co_list_out_p = co_list_out_p->next)
                {
                    if(!runicast_is_transmitting(&runicast)) // Si runicast no esta TX, entra
                    {
                        llenar_connect_msg (&co_msg, co_list_out_p->co_msg.level, &co_list_out_p->co_msg.destination);
                        packetbuf_copyfrom(&co_msg, sizeof(co_msg));
                        packetbuf_set_attr(PACKETBUF_ATTR_PACKET_GHS_TYPE_MSG, CONNECT);
                        runicast_send(&runicast, &co_msg.destination, MAX_RETRANSMISSIONS);
                        stats_ghs.connect_sent++;
                        MY_DBG_1("stats %d %lu %lu %lu %lu %lu %lu %lu\n",linkaddr_node_addr.u8[0], stats_ghs.connect_sent,
                                                                                                  stats_ghs.initiate_sent,
                                                                                                  stats_ghs.test_sent,
                                                                                                  stats_ghs.accept_sent,
                                                                                                  stats_ghs.reject_sent,
                                                                                                  stats_ghs.report_sent,
                                                                                                  stats_ghs.changeroot_sent);

                        MY_DBG_3("Envio CONECT to %d , level=%d \n", co_msg.destination.u8[0], co_msg.level);

                        //remuevo el elemento de la lista
                        my_list_remove(co_list_out, co_list_out_p); //Remove a specific element from a list.
                        memb_free(&co_mem_out, co_list_out_p);

                    }else //Si runicast esta ocupado TX, pospongo el envio del msg
                    {
                        //pospone sending the message
                        process_post(PROCESS_CURRENT(), e_msg_connect, NULL);
                        break; //para salirse del for (co_list_out_p)
                    }
                } //END for recorrer la lista
            } //END si hay elementos en la lista
        }else
        if(ev == e_msg_initiate)
        {
            if(list_length(i_list_out))
            {
                for(i_list_out_p = list_head(i_list_out); i_list_out_p != NULL; i_list_out_p = i_list_out_p->next)
                {
                    if(!runicast_is_transmitting(&runicast)) // Si runicast no esta TX, entra
                    {

                        llenar_initiate_msg(&i_msg,
                                            i_list_out_p->i_msg.f.name_str,
                                            i_list_out_p->i_msg.f.level,
                                            i_list_out_p->i_msg.nd_state,
                                            &i_list_out_p->i_msg.destination,
                                            i_list_out_p->i_msg.flags );
                        packetbuf_copyfrom(&i_msg, sizeof(i_msg));
                        packetbuf_set_attr(PACKETBUF_ATTR_PACKET_GHS_TYPE_MSG, INITIATE);
                        runicast_send(&runicast, &i_msg.destination, MAX_RETRANSMISSIONS);
                        stats_ghs.initiate_sent++;
                        MY_DBG_1("stats %d %lu %lu %lu %lu %lu %lu %lu\n",linkaddr_node_addr.u8[0], stats_ghs.connect_sent,
                                                                                                  stats_ghs.initiate_sent,
                                                                                                  stats_ghs.test_sent,
                                                                                                  stats_ghs.accept_sent,
                                                                                                  stats_ghs.reject_sent,
                                                                                                  stats_ghs.report_sent,
                                                                                                  stats_ghs.changeroot_sent);

                        MY_DBG_3("Envio initiate a %d level= %d name=%d.%02d i_msg.flags=%04X\n", i_msg.destination.u8[0],
                        i_msg.f.level,
                        (int)(i_msg.f.name_str.weight / SEQNO_EWMA_UNITY),
                        (int)(((100UL * i_msg.f.name_str.weight) / SEQNO_EWMA_UNITY) % 100),
                         i_msg.flags);

                         //remuevo el elemento de la lista
                         my_list_remove(i_list_out, i_list_out_p); //Remove a specific element from a list.
                         memb_free(&i_mem_out, i_list_out_p);
                    }else //Si runicast esta ocupado TX, pospongo el envio del msg
                    {
                        //pospone sending the message
                        process_post(PROCESS_CURRENT(), e_msg_initiate, NULL);
                        break; //para salirse del for i_list_out_p
                    }
                } //END for
            } //END if hay elementos en la lista
        }
    } //END of while
    PROCESS_END();
}
