
#ifndef GHS_FIND_FOUND_H
#define GHS_FIND_FOUND_H

/*-------------------------------------------------------------------*/
/*---------------- INCLUDES -----------------------------------------*/
/*-------------------------------------------------------------------*/

#include <stdio.h>
#include "ghs_find_found.h"
#include "net/rime/rime.h" //Aca esta ghs_neigh.h


/*-------------------------------------------------------------------*/
/*---------------- TYPEDEF ------------------------------------------*/
/*-------------------------------------------------------------------*/
typedef struct node node;
typedef struct fragment fragment;
typedef struct LWOE_type LWOE_type;
typedef struct LWOE LWOE;
typedef struct reports reports;
typedef struct test_msg test_msg;

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
    uint8_t weight;
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
    uint8_t weight;
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

/*-------------------------------------------------------------------*/
/*---------------- Variables globales--------------------------------*/
/*-------------------------------------------------------------------*/
node n;

#endif /* GHS_FIND_FOUND_H */
