/*-------------------------------------------------------------------*/
/*---------------- INCLUDES -----------------------------------------*/
/*-------------------------------------------------------------------*/

#include "ghs_test_ar.h"




/*-------------------------------------------------------------------*/
/*---------------- FUNCIONES ----------------------------------------*/
/*-------------------------------------------------------------------*/




/* Retorna el peso de un edge
*/
uint32_t return_weight(edges *e_list_head_g,  const linkaddr_t *from)
{
    edges *e_aux = NULL;
    for(e_aux = e_list_head_g; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
    {
        if(linkaddr_cmp(&e_aux->addr, from)) //Entra si las direcciones son iguales
        {
            break;
        }
    }
    return (e_aux->weight);
}


/* Hace que el edge se vuelva rejected
*/
void become_rejected(edges *e_list_head_g, const linkaddr_t *from)
{
    edges *e_aux;
    for(e_aux = e_list_head_g; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
    {
        if(linkaddr_cmp(&e_aux->addr, from)) //Entra si las direcciones son iguales
        {
            e_aux->state = E_REJECTED;
            break;
        }
    }

}
/* Hace que el edges se vuelva accepted
*/
void become_accepted(edges *e_list_head_g, const linkaddr_t *from)
{
    edges *e_aux;
    for(e_aux = e_list_head_g; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
    {
        if(linkaddr_cmp(&e_aux->addr, from)) //Entra si las direcciones son iguales
        {
            e_aux->state = E_ACCEPTED;
            break;
        }
    }
}

/* Funcion para llenar el msg test
*/
void llenar_test_msg (test_msg *t_msg, const linkaddr_t *destination, fragment f)
{
    linkaddr_copy(&t_msg->destination,  destination);
    t_msg->f.name_str      = f.name_str;
    t_msg->f.level         = f.level;
}

void llenar_test_msg_list (test_list *t_list_out_p, const linkaddr_t *destination, fragment f)
{
    linkaddr_copy(&t_list_out_p->t_msg.destination,  destination);
    t_list_out_p->t_msg.f.name_str      = f.name_str;
    t_list_out_p->t_msg.f.level         = f.level;
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
        return 1;
    }else
    {
        return 0;
    }
}
