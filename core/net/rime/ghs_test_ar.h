#ifndef GHS_TEST_AR_H
#define GHS_TEST_AR_H

/*-------------------------------------------------------------------*/
/*---------------- INCLUDES -----------------------------------------*/
/*-------------------------------------------------------------------*/


#include "contiki.h"
#include "ghs_algorithm.h"
#include "net/rime/rime.h" //Aca esta ghs_neigh.h
#include <stdio.h>
//#include <inttypes.h> //para imprimir el INFINITO. NO BORRAR!

/*-------------------------------------------------------------------*/
/*---------------- Definiciones -------------------------------------*/
/*-------------------------------------------------------------------*/

#define INFINITO UINT32_MAX
/*-------------------------------------------------------------------*/
/*---------------- TYPEDEF ------------------------------------------*/
/*-------------------------------------------------------------------*/
typedef struct fragment fragment;
typedef struct test_msg test_msg;
typedef struct accept_msg accept_msg;
typedef struct reject_msg reject_msg;
typedef struct edges edges;
typedef struct pospone_test pospone_test;
typedef struct report_msg report_msg;
typedef struct report_str report_str;

/*-------------------------------------------------------------------*/
/*---------------- EVENTOS ------------------------------------------*/
/*-------------------------------------------------------------------*/
//estados
process_event_t e_init_master_test_ar;
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


void init_master_test_ar(struct process *master_co_i, struct process *send_message_test_ar,
                         struct process *e_pospone_test, struct process *e_test,
                         struct process *send_message_report_ChaRoot,
                         struct process *reports_completos, struct process *e_LWOE);

void llenar_test_msg (test_msg *t_msg, const linkaddr_t *destination, fragment f);
void llenar_accept_msg (accept_msg *a_msg, const linkaddr_t *destination);
void llenar_reject_msg (reject_msg *r_msg, const linkaddr_t *destination);
void become_accepted(edges *e_list_head_g, const linkaddr_t *from);
void become_rejected(edges *e_list_head_g, const linkaddr_t *from);
void llenar_pospone_test(pospone_test *pt, const linkaddr_t *neighbor, test_msg t_msg);
uint32_t return_weight(edges *e_list_head_g,  const linkaddr_t *from);
void llenar_report_msg(report_msg *rp_msg, const linkaddr_t *destination,
                      const linkaddr_t *neighbor_r, uint32_t weight_r);

void ghs_test_ar_recv_ruc(void *msg, struct history_entry *h_entry_head, const linkaddr_t *from,
                         struct memb *history_mem, list_t history_list, uint8_t seqno,
                         struct process *send_message_test_ar, edges *e_list_head_g,
                         list_t pt_list, struct memb *pt_memb, struct process *master_test_ar,
                         struct process *e_test, struct process *send_message_report_ChaRoot,
                         struct process *master_co_i, struct process *e_LWOE);

#endif /* GHS_NEIGH_H */
