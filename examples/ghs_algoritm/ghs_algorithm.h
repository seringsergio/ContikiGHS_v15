#ifndef GHS_ALGORITHM_H
#define GHS_ALGORITHM_H

/*------------------------------------------------------------------- */
/*----------- INCLUDES ------------------------------------------------ */
/*------------------------------------------------------------------- */

#include "contiki.h"


/*------------------------------------------------------------------- */
/*-----------TYPEDEF-------------------------------------------------*/
/*------------------------------------------------------------------- */
typedef struct wait s_wait;
/*------------------------------------------------------------------- */
/*-----------PROCESOS-------------------------------------------------*/
/*------------------------------------------------------------------- */

//Procesos generales
PROCESS_NAME(wait);

//Procesos de neighbor_discovery
PROCESS_NAME(n_broadcast_neighbor_discovery);
PROCESS_NAME(n_link_weight_worst_case);
PROCESS_NAME(master_neighbor_discovery);
PROCESS_NAME(master_find_found);

//Procesos de find found
PROCESS_NAME(send_message);
PROCESS_NAME(e_pospone_connect);    

/*------------------------------------------------------------------- */
/*----------EVENTOS -------- -----------------------------------------*/
/*------------------------------------------------------------------- */

//Comunes a todos los procesos
extern process_event_t e_wait_stabilization;
extern process_event_t e_infinite_wait;

//neighbor discovery
extern process_event_t e_discovery_broadcast;
extern process_event_t e_weight_worst;
extern process_event_t e_init_find_found;

// master find found
    //estados
extern process_event_t e_found;
extern process_event_t e_find;
    //msg
extern process_event_t e_msg_connect;
extern process_event_t e_msg_initiate;
extern process_event_t e_msg_test;
extern process_event_t e_msg_reject;
extern process_event_t e_msg_accept;
extern process_event_t e_msg_report;
extern process_event_t e_msg_change_root;
/*-------------------------------------------------------------------*/
/*---------------- Estructuras---------------------------------------*/
/*-------------------------------------------------------------------*/

struct wait
{
    uint8_t seconds;
    struct process *return_process;
};


#endif /* GHS_ALGORITHM_H */
