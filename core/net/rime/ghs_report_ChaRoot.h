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
typedef struct change_root_msg change_root_msg;
typedef struct info_found info_found;
/*-------------------------------------------------------------------*/
/*---------------- EVENTOS ------------------------------------------*/
/*-------------------------------------------------------------------*/
//estados
//process_event_t e_init_master_report_ChaRoot;

//msg
process_event_t e_msg_report;
//process_event_t e_msg_ch_root;
/*-------------------------------------------------------------------*/
/*---------------- ESTRUCTURAS---------------------------------------*/
/*-------------------------------------------------------------------*/
struct change_root_msg
{
    linkaddr_t next_hop; //El siguiente hop en el camino del msg de change root
    linkaddr_t final_destination; //El destino final del msg de change root

};


struct report_msg
{
    linkaddr_t destination; //Hacia donde envio el msg de report
    linkaddr_t quien_reporto; //Neighbor con LWOE dentro del fragmento. El nodo quien reporto
    uint32_t weight_r; //Este es el peso del nodo con menor peso. Weight_reportado
};

struct report_str
{
    struct report_str *next;
    report_msg rp_msg;
    linkaddr_t neighbor; //Vecino que envio el msg de report
};

struct info_found
{
    list_t report_list;
    struct memb *report_memb;
};
/*-------------------------------------------------------------------*/
/*---------------- Variables globales--------------------------------*/
/*-------------------------------------------------------------------*/


/*-------------------------------------------------------------------*/
/*----------------DECLARACION FUNCIONES -----------------------------*/
/*-------------------------------------------------------------------*/

void ghs_report_ChaRoot_recv_ruc(void *msg, struct history_entry *h_entry_head, const linkaddr_t *from,
                         struct memb *history_mem, list_t history_list, uint8_t seqno,
                         struct memb *report_memb, list_t report_list, struct process *reports_completos,
                         struct process *send_message_co_i, struct process *send_message_report_ChaRoot);

void llenar_change_root(change_root_msg *cr_msg, const linkaddr_t *next_hop,
                        const linkaddr_t *final_destination);


#endif /* GHS_NEIGH_H */
