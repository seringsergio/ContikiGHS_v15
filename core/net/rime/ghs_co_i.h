
#ifndef GHS_CO_I_H
#define GHS_CO_I_H

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
//Definicion de maximos
#define MAX_NUM_EDGES     MAX_NEIGHBORS
#define MAX_NUM_POSPONES  MAX_NEIGHBORS

//Estados de los nodos
#define FIND  0x01
#define FOUND 0x02
#define END   0x04

//Banderas de los nodos
#define INITIATIOR  0x01
#define P_LWOE      0x02
#define C_LWOE      0x04
#define CORE_NODE   0x08

//Estados de los edges
#define BASIC      0x01
#define BRANCH     0x02
#define E_REJECTED 0x04
#define E_ACCEPTED 0x08

//Tipos de mensajes
#define CONNECT      0x01
#define INITIATE     0x02
#define TEST         0x04
#define M_REJECT     0x08
#define M_ACCEPT     0x10
#define REPORT       0x20
#define CHANGE_ROOT  0x40


/*-------------------------------------------------------------------*/
/*---------------- TYPEDEF ------------------------------------------*/
/*-------------------------------------------------------------------*/
typedef struct node node;
typedef struct fragment fragment;
typedef struct LWOE_type LWOE_type;
typedef struct LWOE LWOE;
typedef struct reports reports;
typedef struct test_msg test_msg;
typedef struct edges edges;
typedef struct connect_msg connect_msg;
typedef struct initiate_msg initiate_msg;
typedef struct pospone_connect pospone_connect;
/*-------------------------------------------------------------------*/
/*---------------- EVENTOS ------------------------------------------*/
/*-------------------------------------------------------------------*/
// estados
process_event_t e_found;
process_event_t e_find;

// msg
process_event_t e_msg_connect;
process_event_t e_msg_initiate;
process_event_t e_msg_test;
process_event_t e_msg_reject;
process_event_t e_msg_accept;
process_event_t e_msg_report;
process_event_t e_msg_change_root;


/*-------------------------------------------------------------------*/
/*---------------- ESTRUCTURAS---------------------------------------*/
/*-------------------------------------------------------------------*/

struct fragment
{
    uint32_t name;
    uint8_t level;

};

struct LWOE
{
    linkaddr_t neighbor;
    uint32_t weight;
};

struct LWOE_type
{
    LWOE node;
    LWOE children;
};

struct reports
{
    linkaddr_t sender;
    linkaddr_t neighbor;
    uint32_t weight;
    uint16_t num_reports;
};

struct test_msg
{
    uint8_t type;
    linkaddr_t sender;
    fragment f;
};


struct node
{
    uint8_t state;
    uint8_t flags;
    fragment f;
    linkaddr_t parent; //Para enviar msg en la downward direction
    LWOE_type lwoe;
    reports r;
    uint8_t num_children;
    linkaddr_t downroute; //Para enviar msg en la downward direction
    test_msg t_msg;
    uint16_t num_test;

};

// Es una lista con la informacion de los edges
struct edges {

  struct edges *next;   // The ->next pointer is needed since we are placing these on a Contiki list.
  uint8_t state; // Estado del edge
  linkaddr_t addr;   // The ->addr field holds the Rime address of the neighbor.
  uint32_t weight; //Este es el peso del nodo
};

/*-------------------------------------------------------------------*/
/*---------------- ESTRUCTURAS MSG-----------------------------------*/
/*-------------------------------------------------------------------*/

struct initiate_msg
{
    fragment f;
    uint8_t nd_state;
    linkaddr_t destination;
};

struct connect_msg
{
    uint8_t level;
    linkaddr_t destination;
};

struct pospone_connect
{
    struct pospone_connect *next;
    connect_msg co_msg;  //msg de connect
    linkaddr_t neighbor; //Vecino que envio el msg de connect

};

/*-------------------------------------------------------------------*/
/*---------------- Variables globales--------------------------------*/
/*-------------------------------------------------------------------*/
extern node nd; //nd es node....n es neighbor
extern pospone_connect pc; //pc = pospone connect

/*-------------------------------------------------------------------*/
/*---------------- FUNCIONES ----------------------------------------*/
/*-------------------------------------------------------------------*/
void fill_edges_list(list_t edges_list, struct memb *edges_memb, struct neighbor *n_list_head);
void print_edges_list(edges *e_list_head, char *string,  const linkaddr_t *node_addr);
void become_branch(edges *e_list_head, const linkaddr_t *node_addr);
linkaddr_t* least_basic_edge(edges *e_list_head);

void ghs_ff_recv_ruc(void *msg, const linkaddr_t *from,
                    struct memb *history_mem, list_t history_list, uint8_t seqno,
                    node *nd,  edges *e_list_head, struct process *send_message_co_i,
                    struct memb *pc_memb  ,list_t pc_list, struct process *master_co_i);

void ghs_ff_send_ruc(const linkaddr_t *to, uint8_t retransmissions);
void ghs_ff_timedout_ruc(const linkaddr_t *to, uint8_t retransmissions);
void init_master_co_i(struct neighbor *n_list_head, struct process *master_neighbor_discovery,
                        struct process *send_message_co_i, struct process *e_pospone_connect, node *nd,
                        struct memb *edges_memb, list_t edges_list, const linkaddr_t *node_addr);
uint8_t state_is_branch( const linkaddr_t *addr,  edges *e_list_head);
uint32_t weight_with_edge( const linkaddr_t *addr,  edges *e_list_head);
void llenar_initiate_msg(initiate_msg *i_msg, uint32_t name,
                        uint8_t level, uint8_t state, const linkaddr_t *dest);
void llenar_connect_msg (connect_msg *msg, uint8_t level, linkaddr_t *destination);
void llenar_pospone_connect(pospone_connect *pc, const linkaddr_t *neighbor, connect_msg co_msg);


#endif /* GHS_CO_I_H */
