
#ifndef GHS_CO_I_H
#define GHS_CO_I_H

/*-------------------------------------------------------------------*/
/*---------------- INCLUDES -----------------------------------------*/
/*-------------------------------------------------------------------*/
#include "contiki.h"
#include "ghs_algorithm.h"
#include "net/rime/rime.h"
#include <stdio.h>

/*-------------------------------------------------------------------*/
/*---------------- Definiciones -------------------------------------*/
/*-------------------------------------------------------------------*/

//#define WAIT_RARO 6
//Definicion de maximos
#define MAX_NUM_EDGES           MAX_NEIGHBORS
#define MAX_NUM_POSPONES        MAX_NEIGHBORS
#define MAX_TAMANO_LISTA_MSG    MAX_NEIGHBORS //lista donde guardo msgs (co-i-test-ar-report-Chroot)

//Estados de los nodos
#define FIND  0x01
#define FOUND 0x02
#define END   0x04

//Banderas de los nodos nd.flags
#define INITIATIOR  0x01
#define ND_LWOE     0x02
#define CH_LWOE     0x04
#define CORE_NODE   0x08

//Banderas de initiate_msg
#define BECOME_CORE_NODE 0x01

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
//typedef struct reports reports;

typedef struct test_msg test_msg;
typedef struct edges edges;
typedef struct connect_msg connect_msg;
typedef struct initiate_msg initiate_msg;
typedef struct accept_msg accept_msg;
typedef struct reject_msg reject_msg;
typedef struct report_msg report_msg;
typedef struct info_found info_found;
typedef struct connect_list connect_list;
typedef struct initiate_list initiate_list;
typedef struct accept_list accept_list;

/*-------------------------------------------------------------------*/
/*---------------- EVENTOS ------------------------------------------*/
/*-------------------------------------------------------------------*/
// estados
process_event_t e_found;
process_event_t e_find;

// msg
process_event_t e_msg_connect;
process_event_t e_msg_initiate;


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

/*struct reports
{
    linkaddr_t sender;
    linkaddr_t neighbor;
    uint32_t weight;
    uint16_t num_reports;
};*/


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
    uint8_t flags;
    uint8_t nd_state;
    linkaddr_t destination;
};

struct initiate_list
{
    struct initiate_list *next;
    initiate_msg i_msg;
    linkaddr_t from;
};

struct connect_msg
{
    uint8_t level;
    linkaddr_t destination;
};

struct connect_list
{
    struct connect_list *next;
    connect_msg co_msg;
    linkaddr_t from;
};

struct test_msg
{
    linkaddr_t destination;
    fragment f;
};

struct test_list
{
    struct test_list *next;
    test_msg t_msg;
    linkaddr_t from;
};

struct accept_msg
{
    linkaddr_t destination;
};

struct accept_list
{
    struct accept_list *next;
    linkaddr_t from;
};

struct reject_msg
{
    linkaddr_t destination;
};

struct node
{
    uint8_t state;
    uint8_t flags;
    fragment f;
    linkaddr_t parent; //Para enviar msg en la upward direction
    LWOE_type lwoe;
    //reports r;
    uint8_t num_children;
    linkaddr_t downroute; //Para enviar msg en la downward direction
    test_msg t_msg;
    linkaddr_t otro_core_node; //Indica la direccion del otro CORE_NODE

};
/*-------------------------------------------------------------------*/
/*---------------- Variables globales--------------------------------*/
/*-------------------------------------------------------------------*/
extern node nd; //nd es node....n es neighbor

/*-------------------------------------------------------------------*/
/*---------------- FUNCIONES ----------------------------------------*/
/*-------------------------------------------------------------------*/
void fill_edges_list(list_t edges_list, struct memb *edges_memb, struct neighbor *n_list_head);
void print_edges_list(edges *e_list_head, char *string,  const linkaddr_t *node_addr);
void become_branch(edges *e_list_head, const linkaddr_t *node_addr);
linkaddr_t* least_basic_edge(edges *e_list_head);

void ghs_co_i_recv_ruc(void *msg, const linkaddr_t *from,
                    struct memb *history_mem, list_t history_list, uint8_t seqno,
                    list_t co_list, struct memb *co_mem, struct process *evaluar_msg_co,
                    list_t i_list, struct memb *i_mem, struct process *evaluar_msg_i,
                    struct process *evaluar_msg_test);

void init_master_co_i(struct neighbor *n_list_head, struct memb *edges_memb, list_t edges_list);
uint8_t state_is_branch( const linkaddr_t *addr,  edges *e_list_head);
uint32_t weight_with_edge( const linkaddr_t *addr,  edges *e_list_head);
void llenar_initiate_msg(initiate_msg *i_msg, uint32_t name,
                        uint8_t level, uint8_t state, const linkaddr_t *dest, uint8_t flags);
void llenar_connect_msg (connect_msg *msg, uint8_t level, linkaddr_t *destination);



#endif /* GHS_CO_I_H */
