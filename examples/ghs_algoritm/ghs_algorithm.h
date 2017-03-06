#ifndef GHS_ALGORITHM_H
#define GHS_ALGORITHM_H

/*------------------------------------------------------------------- */
/*-----------PROCESOS-------------------------------------------------*/
/*------------------------------------------------------------------- */
PROCESS_NAME(wait);
PROCESS_NAME(n_broadcast_neighbor_discovery);
PROCESS_NAME(n_link_weight_worst_case);
PROCESS_NAME(master_neighbor_discovery);
PROCESS_NAME(master_find_found);

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


#endif /* GHS_ALGORITHM_H */
