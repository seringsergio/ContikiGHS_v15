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

typedef struct report_list report_list;
typedef struct report_msg report_msg;
typedef struct change_root_msg change_root_msg;
typedef struct info_found info_found;
typedef struct change_root_list change_root_list;

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

struct change_root_list
{
    struct change_root_list *next;
    change_root_msg cr_msg;
    linkaddr_t from;
};

struct report_msg
{
    linkaddr_t destination; //Hacia donde envio el msg de report
    linkaddr_t neighbor_r; //Neighbor con LWOE dentro del fragmento. El nodo quien reporto
    uint32_t weight_r; //Este es el peso del nodo con menor peso. Weight_reportado
};

struct report_list
{
    struct report_list *next;
    report_msg rp_msg;
    linkaddr_t from; //Vecino que envio el msg de report
};

struct info_found
{
    list_t rp_list;
    struct memb *rp_mem;
};
/*-------------------------------------------------------------------*/
/*---------------- Variables globales--------------------------------*/
/*-------------------------------------------------------------------*/


/*-------------------------------------------------------------------*/
/*----------------DECLARACION FUNCIONES -----------------------------*/
/*-------------------------------------------------------------------*/

void ghs_report_ChaRoot_recv_ruc(void *msg,
                         const linkaddr_t *from,
                         struct memb *history_mem, list_t history_list, uint8_t seqno,
                         struct memb *rp_mem, list_t rp_list, struct process *evaluar_msg_rp,
                         list_t  cr_list, struct memb *cr_mem, struct process *evaluar_msg_cr );

void llenar_change_root(change_root_msg *cr_msg, const linkaddr_t *next_hop,
                        const linkaddr_t *final_destination);

uint8_t es_Hoja();

void llenar_report_msg(report_msg *rp_msg, const linkaddr_t *destination,
                      const linkaddr_t *neighbor_r, uint32_t weight_r);

uint8_t lista_casi_completa( list_t rp_list);

#endif /* GHS_NEIGH_H */
