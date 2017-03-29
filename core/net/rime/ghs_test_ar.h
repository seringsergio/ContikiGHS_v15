#ifndef GHS_TEST_AR_H
#define GHS_TEST_AR_H

/*-------------------------------------------------------------------*/
/*---------------- INCLUDES -----------------------------------------*/
/*-------------------------------------------------------------------*/


#include "contiki.h"
#include "ghs_algorithm.h"
#include "net/rime/rime.h"
#include <stdio.h>

//#include <inttypes.h> //para imprimir el INFINITO. NO BORRAR!

/*-------------------------------------------------------------------*/
/*---------------- Definiciones -------------------------------------*/
/*-------------------------------------------------------------------*/

#define INFINITO UINT32_MAX
#define MIN_CLOCK_SECOND 128
/*-------------------------------------------------------------------*/
/*---------------- TYPEDEF ------------------------------------------*/
/*-------------------------------------------------------------------*/
typedef struct fragment fragment;
typedef struct test_msg test_msg;
typedef struct test_list test_list;
typedef struct accept_msg accept_msg;
typedef struct reject_msg reject_msg;
typedef struct edges edges;
typedef struct report_msg report_msg;
typedef struct report_list report_list;


/*-------------------------------------------------------------------*/
/*---------------- EVENTOS ------------------------------------------*/
/*-------------------------------------------------------------------*/
//estados
process_event_t e_evaluate_test;
process_event_t e_nd_lwoe; //Para saber si el nodo ya tiene su edge preferido
process_event_t e_ch_lwoe; // Para saber si los hijos ya reportaton el edge preferido


//msg
process_event_t e_msg_test;
process_event_t e_msg_reject;
process_event_t e_msg_accept;
process_event_t e_msg_ch_root;

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

void llenar_test_msg (test_msg *t_msg, const linkaddr_t *destination, fragment f);
void llenar_accept_msg (accept_msg *a_msg, const linkaddr_t *destination);
void llenar_reject_msg ( reject_msg *r_msg,  linkaddr_t *destination);
void become_accepted(edges *e_list_head_g, const linkaddr_t *from);
void become_rejected(edges *e_list_head_g, const linkaddr_t *from);
uint32_t return_weight(edges *e_list_head_g,  const linkaddr_t *from);

void ghs_test_ar_recv_ruc(void *msg, const linkaddr_t *from,
                         struct memb *history_mem, list_t history_list, uint8_t seqno,
                         struct process *e_test,
                         struct memb *t_mem, list_t t_list, struct process *evaluar_msg_test,
                         struct memb *a_mem, list_t a_list, struct process *evaluar_msg_accept,
                         list_t rj_list, struct memb *rj_mem, struct process *evaluar_msg_reject);



#endif /* GHS_NEIGH_H */
