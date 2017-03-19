#ifndef GHS_REPORT_CHAROOT_H
#define GHS_REPORT_CHAROOT_H


/*-------------------------------------------------------------------*/
/*---------------- INCLUDES -----------------------------------------*/
/*-------------------------------------------------------------------*/


#include "contiki.h"
#include "ghs_algorithm.h"
#include "net/rime/rime.h" //Aca esta ghs_neigh.h
#include <stdio.h>
#include "ghs_algorithm.h"



/*-------------------------------------------------------------------*/
/*---------------- Definiciones -------------------------------------*/
/*-------------------------------------------------------------------*/
#define MAX_NUM_REPORTS MAX_NEIGHBORS


/*-------------------------------------------------------------------*/
/*---------------- TYPEDEF ------------------------------------------*/
/*-------------------------------------------------------------------*/

typedef struct report_str report_str;
typedef struct report_msg report_msg;

/*-------------------------------------------------------------------*/
/*---------------- EVENTOS ------------------------------------------*/
/*-------------------------------------------------------------------*/
//estados
process_event_t e_init_master_report_ChaRoot;

//msg
process_event_t e_msg_report;

/*-------------------------------------------------------------------*/
/*---------------- ESTRUCTURAS---------------------------------------*/
/*-------------------------------------------------------------------*/

struct report_msg
{
    linkaddr_t destination; //Hacia donde envio el msg de report
    linkaddr_t neighbor_r; //Neighbor con menor peso. Neighbor_reportado
    uint32_t weight_r; //Este es el peso del nodo con menor peso. Weight_reportado
};

struct report_str
{
    struct report_str *next;
    report_msg rp_msg;
    linkaddr_t neighbor; //Vecino que envio el msg de report
};
/*-------------------------------------------------------------------*/
/*---------------- ESTRUCTURAS MSG-----------------------------------*/
/*-------------------------------------------------------------------*/


/*-------------------------------------------------------------------*/
/*---------------- Variables globales--------------------------------*/
/*-------------------------------------------------------------------*/


/*-------------------------------------------------------------------*/
/*----------------DECLARACION FUNCIONES -----------------------------*/
/*-------------------------------------------------------------------*/

void ghs_report_ChaRoot_recv_ruc(void *msg, struct history_entry *h_entry_head, const linkaddr_t *from,
                         struct memb *history_mem, list_t history_list, uint8_t seqno,
                         struct memb *report_memb, list_t report_list, struct process *reports_completos);



#endif /* GHS_NEIGH_H */
