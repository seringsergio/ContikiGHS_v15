/*-------------------------------------------------------------------*/
/*---------------- INCLUDES -----------------------------------------*/
/*-------------------------------------------------------------------*/

#include "ghs_test_ar.h"
/*-------------------------------------------------------------------*/
/*---------------- FUNCIONES ----------------------------------------*/
/*-------------------------------------------------------------------*/

/* Hace que el edge se vuelva rejected
*/
uint8_t become_rejected(edges *e_list_head_g, const linkaddr_t *from)
{
    edges *e_aux = NULL;
    uint8_t temp = 0;

    for(e_aux = e_list_head_g; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
    {
        if(linkaddr_cmp(&e_aux->addr, from)) //Entra si las direcciones son iguales
        {
            if( (e_aux->state == BASIC) || (e_aux->state == E_ACCEPTED) || (e_aux->state == E_REJECTED) )
            {
                e_aux->state = E_REJECTED;
                temp = 1;
                break;
            }else
            if(e_aux->state == BRANCH)
            {
                MY_DBG("ERROR: Preguntarse porque quiero volver rejected un edge q ya es BRANCH\n");
                temp = 0;
                break;
            }
        }
    }//end for

    if( e_aux == NULL)
    {
        MY_DBG("ERROR: El vecino no existe en la lista de Edges\n");
    }

    return temp;
}
/* Hace que el edges se vuelva accepted
*/
uint8_t become_accepted(edges *e_list_head_g, const linkaddr_t *from)
{
    edges *e_aux = NULL;
    uint8_t temp = 0;

    for(e_aux = e_list_head_g; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
    {
        if(linkaddr_cmp(&e_aux->addr, from)) //Entra si las direcciones son iguales
        {
            //SOlamente puede volverse accepted si esta en el estado BASIC o ya es E_ACCEPTED
            if( (e_aux->state == BASIC) || (e_aux->state == E_ACCEPTED) )
            {
                e_aux->state = E_ACCEPTED;
                temp = 1;
                break;
            }else
            if( (e_aux->state == BRANCH) || (e_aux->state == E_REJECTED)  )
            {
                MY_DBG("ERROR: Preguntarse porque quiero volver accepted un edge q ya es BRANCH o E_REJECTED\n");
                temp = 0;
                break;
            }
        }
    }

    if( e_aux == NULL)
    {
        MY_DBG("ERROR: El vecino no existe en la lista de Edges\n");
    }

    return temp;
}


/* Funcion para llenar el msg test
*/
void llenar_test_msg (test_msg *t_msg, const linkaddr_t *destination, fragment f)
{
    linkaddr_copy(&t_msg->destination,  destination);
    t_msg->f = f; //Genera errores con esta linea directa...muuuy raro
}

void llenar_test_msg_list (test_list *t_list_out_p, const linkaddr_t *destination, fragment f)
{
    linkaddr_copy(&t_list_out_p->t_msg.destination,  destination);
    t_list_out_p->t_msg.f      = f;
}

/* Funcion para llenar el msg de accept
*/
void llenar_accept_msg (accept_msg *a_msg, const linkaddr_t *destination)
{
    linkaddr_copy(&a_msg->destination,  destination);
}

void llenar_accept_msg_list (accept_list *a_list_out_p, const linkaddr_t *destination)
{
    linkaddr_copy(&a_list_out_p->a_msg.destination,  destination);
}

/* Funcion para llenar el msg de reject
*/
void llenar_reject_msg ( reject_msg *r_msg,  linkaddr_t *destination)
{
    linkaddr_copy(&r_msg->destination,  destination);
}

void llenar_reject_msg_list ( reject_list *rj_list_out_p,  linkaddr_t *destination)
{
    linkaddr_copy(&rj_list_out_p->rj_msg.destination,  destination);
}

uint8_t nombres_iguales(name *name_str_1, name *name_str_2)
{
    if( (name_str_1->weight == name_str_2->weight)                      &&
        (linkaddr_cmp(&name_str_1->core_node_1, &name_str_2->core_node_1)  )  &&
        (linkaddr_cmp(&name_str_1->core_node_2, &name_str_2->core_node_2)  )   )

    {
        MY_DBG("Los nombres son iguales\n");
        return 1;
    }else
    {
        MY_DBG("Los nombres son diferentes\n");
        return 0;
    }
}
