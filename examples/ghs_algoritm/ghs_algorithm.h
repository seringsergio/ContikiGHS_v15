#ifndef GHS_ALGORITHM_H
#define GHS_ALGORITHM_H

/*------------------------------------------------------------------- */
/*-----------PROCESOS-------------------------------------------------*/
/*------------------------------------------------------------------- */
PROCESS_NAME(wait);
PROCESS_NAME(n_broadcast_neighbor_discovery);
PROCESS_NAME(n_link_weight_worst_case);
PROCESS_NAME(master_neighbor_discovery);
PROCESS_NAME(idle_mio);

/*------------------------------------------------------------------- */
/*----------EVENTOS -------- -----------------------------------------*/
/*------------------------------------------------------------------- */
extern process_event_t e_discovery_broadcast;
extern process_event_t e_wait_stabilization;
extern process_event_t e_weight_worst;
extern process_event_t e_idle;


#endif /* GHS_ALGORITHM_H */
