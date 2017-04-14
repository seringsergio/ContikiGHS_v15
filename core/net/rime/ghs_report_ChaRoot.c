/*-------------------------------------------------------------------*/
/*---------------- INCLUDES -----------------------------------------*/
/*-------------------------------------------------------------------*/

#include <stdio.h>
#include "ghs_report_ChaRoot.h"

/*-------------------------------------------------------------------*/
/*---------------- FUNCIONES ----------------------------------------*/
/*-------------------------------------------------------------------*/


/* Funcion para llenar el msg de change root
*/
void llenar_change_root(change_root_msg *cr_msg, const linkaddr_t *next_hop,
                        const linkaddr_t *final_destination)
{
    linkaddr_copy(&cr_msg->next_hop, next_hop);
    linkaddr_copy(&cr_msg->final_destination, final_destination);

}

void llenar_change_root_list(change_root_list *cr_list_out_p, const linkaddr_t *next_hop,
                        const linkaddr_t *final_destination)
{
    linkaddr_copy(&cr_list_out_p->cr_msg.next_hop, next_hop);
    linkaddr_copy(&cr_list_out_p->cr_msg.final_destination, final_destination);
}

/*Determina si el nodo es hoja o no dependiendo del # de hijos
*/
uint8_t es_Hoja()
{
        if( num_hijos(e_list_head_g) == 0  )
        {
            return 1; //Si tengo 0 hijos soy hoja
        }else
        {
            return 0;// EN todos los otros casos no soy hoja
        }
}
/* llenar el msg de informacion
*/
void llenar_msg_informacion(informacion_msg *inf_msg, uint8_t flags, const linkaddr_t *destination )
{
    inf_msg->flags = flags;
    linkaddr_copy(&inf_msg->destination, destination);
}

void llenar_msg_informacion_list(informacion_list *info_list_out_p, uint8_t flags,
                                 const linkaddr_t *destination )
{
    info_list_out_p->info_msg.flags = flags;
    linkaddr_copy(&info_list_out_p->info_msg.destination, destination);
}

/* LLenar el msg de report
*/
void llenar_report_msg(report_msg *rp_msg, const linkaddr_t *destination,
                      const linkaddr_t *neighbor_r, uint32_t weight_r)
{
    linkaddr_copy(&rp_msg->destination, destination);
    linkaddr_copy(&rp_msg->neighbor_r, neighbor_r);
    rp_msg->weight_r  = weight_r;
}

void llenar_report_msg_list(report_list *rp_list_out_p, const linkaddr_t *destination,
                      const linkaddr_t *neighbor_r, uint32_t weight_r)
{
    linkaddr_copy(&rp_list_out_p->rp_msg.destination, destination);
    linkaddr_copy(&rp_list_out_p->rp_msg.neighbor_r, neighbor_r);
    rp_list_out_p->rp_msg.weight_r  = weight_r;
}

/* Indica que la lista ya esta completa en el caso de un CORE_NODE
*  en el cual todos los hijos reportaron menos el OTRO_CORE_NODE
*/
uint8_t lista_casi_completa( list_t rp_list)
{
    report_list *rp_list_p;
    uint8_t no_falta_core_node = 0;
    uint8_t lista_completa = 0;

// SI SOY CORE_NODE
//SI solamente falta 1 hijo
// Y ese hijo faltante es el OTRO_CORE_NODE
//Entonces: La lista esta CASI completa


    if(nd.flags & CORE_NODE) //SI SOY CORE_NODE
    {
        if( list_length(rp_list) == (num_hijos(e_list_head_g)-1)  )  //ME FALTA SOLAMENTE 1 HIJO
        {
            for(rp_list_p = list_head(rp_list); rp_list_p != NULL; rp_list_p = rp_list_p->next)
            {
                if(linkaddr_cmp(&rp_list_p->from , &nd.otro_core_node)) //Entra si las direcciones son iguales
                {
                    no_falta_core_node = 1;

                }
            }

            if(no_falta_core_node == 1)
            {
                lista_completa = 0; //LISTA incompleta
                printf("NOOO ... Lista NO completa\n");
            }else
            {
                lista_completa = 1; //Lista completa
                printf("Casicompleta porque solamente falta el otro core_node\n");
            }
        }
    } //END si soy CORE_NODE

    return lista_completa;
}
