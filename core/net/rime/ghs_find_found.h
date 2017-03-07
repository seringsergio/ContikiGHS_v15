
#ifndef GHS_FIND_FOUND_H
#define GHS_FIND_FOUND_H

/*-------------------------------------------------------------------*/
/*---------------- INCLUDES -----------------------------------------*/
/*-------------------------------------------------------------------*/

#include <stdio.h>
#include "ghs_find_found.h"
#include "net/rime/rime.h" //Aca esta ghs_neigh.h

/*-------------------------------------------------------------------*/
/*---------------- Definiciones ------------------------------------------*/
/*-------------------------------------------------------------------*/
#define MAX_NUM_EDGES MAX_NEIGHBORS

//Estados de los edges
#define BASIC      0x01
#define BRANCH     0x02
#define E_REJECTED 0x04
#define E_ACCEPTED 0x08
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
/*-------------------------------------------------------------------*/
/*---------------- ESTRUCTURAS---------------------------------------*/
/*-------------------------------------------------------------------*/

struct fragment
{
    uint8_t name;
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
    LWOE_type lwoe_type;
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
/*---------------- Variables globales--------------------------------*/
/*-------------------------------------------------------------------*/
node n;

/*-------------------------------------------------------------------*/
/*---------------- FUNCIONES ----------------------------------------*/
/*-------------------------------------------------------------------*/
void fill_edges_list(list_t edges_list, struct memb *edges_memb, struct neighbor *n_list_head);
void print_edges_list(edges *e_list_head, char *string,  const linkaddr_t *node_addr);
void become_branch(edges *e_list_head, linkaddr_t *node_addr);
linkaddr_t* least_basic_edge(edges *e_list_head);


#endif /* GHS_FIND_FOUND_H */
