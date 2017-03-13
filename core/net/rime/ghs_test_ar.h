#ifndef GHS_TEST_AR_H
#define GHS_TEST_AR_H

/*-------------------------------------------------------------------*/
/*---------------- INCLUDES -----------------------------------------*/
/*-------------------------------------------------------------------*/


#include "contiki.h"
#include "ghs_algorithm.h"
#include "net/rime/rime.h" //Aca esta ghs_neigh.h
#include <stdio.h>

/*-------------------------------------------------------------------*/
/*---------------- Definiciones -------------------------------------*/
/*-------------------------------------------------------------------*/

/*-------------------------------------------------------------------*/
/*---------------- TYPEDEF ------------------------------------------*/
/*-------------------------------------------------------------------*/
typedef struct fragment fragment;
typedef struct test_msg test_msg;
typedef struct accept_msg accept_msg;
typedef struct reject_msg reject_msg;


/*-------------------------------------------------------------------*/
/*---------------- EVENTOS ------------------------------------------*/
/*-------------------------------------------------------------------*/
//estados
process_event_t e_init_master_test_ar;
process_event_t e_evaluate_test;

//msg
process_event_t e_msg_test;
process_event_t e_msg_reject;
process_event_t e_msg_accept;

/*-------------------------------------------------------------------*/
/*---------------- ESTRUCTURAS---------------------------------------*/
/*-------------------------------------------------------------------*/

/*-------------------------------------------------------------------*/
/*---------------- ESTRUCTURAS MSG-----------------------------------*/
/*-------------------------------------------------------------------*/


/*-------------------------------------------------------------------*/
/*---------------- Variables globales--------------------------------*/
/*-------------------------------------------------------------------*/


/*-------------------------------------------------------------------*/
/*----------------DECLARACION FUNCIONES -----------------------------*/
/*-------------------------------------------------------------------*/


void init_master_test_ar(struct process *master_co_i, struct process *send_message_test_ar,
                         struct process *e_pospone_test, struct process *e_test);
void llenar_test_msg (test_msg *t_msg, const linkaddr_t *destination, fragment f);
void llenar_accept_msg (accept_msg *a_msg, const linkaddr_t *destination);
void llenar_reject_msg (reject_msg *r_msg, const linkaddr_t *destination);

void ghs_test_ar_recv_ruc(void *msg, struct history_entry *h_entry_head, const linkaddr_t *from,
                         struct memb *history_mem, list_t history_list, uint8_t seqno,
                         struct process *send_message_test_ar);



#endif /* GHS_NEIGH_H */
