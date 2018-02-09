/*-------------------------------------------------------------------*/
/*---------------- INCLUDES -----------------------------------------*/
/*-------------------------------------------------------------------*/
#include "ghs_co_i.h"
/*-------------------------------------------------------------------*/
/*---------------- FUNCIONES-----------------------------------------*/
/*-------------------------------------------------------------------*/

/* FUNCION que imprime el resultado final y definitivo
*/
void print_final_result()
{

    edges *e_aux;
    char string[] = "END";
    for(e_aux = e_list_head_g; e_aux != NULL; e_aux = e_aux->next) // Recorrer toda la lista
    {
        if(linkaddr_cmp(&e_aux->addr, &nd.parent)) //Solo muestro mi padre
        {
            MY_DBG_1("%s %d %d %d.%02d %d %d.%02d \n",
            string,
            linkaddr_node_addr.u8[0],
            nd.parent.u8[0],
            (int)(e_aux->weight / SEQNO_EWMA_UNITY),
            (int)(((100UL * e_aux->weight) / SEQNO_EWMA_UNITY) % 100),
            e_aux->state,
            (int)(nd.f.name_str.weight / SEQNO_EWMA_UNITY),
            (int)(((100UL * nd.f.name_str.weight) / SEQNO_EWMA_UNITY) % 100));
       }
   }
}


/* Toma la informacion de la lista de vecinos (neighbors_list) del master_neighbor_discovery
*  y copia la informacion de interes en una nueva lista de edges edges_list.
*/
void fill_edges_list(list_t edges_list, struct memb *edges_memb, struct neighbor *n_list_head)
{
    struct neighbor *n_aux;
    edges *e;
    for(n_aux = n_list_head; n_aux != NULL; n_aux = list_item_next(n_aux)) // Recorrer toda la lista
    {
        e = memb_alloc(edges_memb);        // we allocate a new struct edges from the edges_memb memory pool.

        if(e == NULL)
        {
          MY_DBG_1("ERROR: No hay espacio para allocar memoria (fill_edges_list) \n");
        }else
        {
            e->state = BASIC; //Todos los edges inician con estado BASIC
            linkaddr_copy(&e->addr,  &n_aux->addr);
            e->weight = n_aux->avg_seqno_gap;

            list_add(edges_list, e); //Add an item at the end of a list
        }
    } //END for

}


/* Imprime la lista de edges
*/
void print_edges_list(edges *e_list_head, char *string,  const linkaddr_t *node_addr)
{
    edges *e_aux = NULL;

    for(e_aux = e_list_head; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
    {
        MY_DBG_1("%s %d %d %d.%02d %d \n",
              string,
              node_addr->u8[0],
              e_aux->addr.u8[0],
              (int)(e_aux->weight / SEQNO_EWMA_UNITY),
              (int)(((100UL * e_aux->weight) / SEQNO_EWMA_UNITY) % 100),
              e_aux->state
              );
    }
}

/* Un edge pasa de estado BASIC/ACCEPT a BRANCH.
*  Become_branch = Vuelve branch un edge
*/
void become_branch(edges *e_list_head, const linkaddr_t *node_addr)
{
    edges *e_aux = NULL;
    for(e_aux = e_list_head; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
    {
        if(linkaddr_cmp(&e_aux->addr, node_addr)) //Entra si las direcciones son iguales
        {
            if(e_aux->state != E_REJECTED)
            {
              e_aux->state = BRANCH;
              break;
            }else
            {
              MY_DBG_1("ERROR: Preguntarse porque quiero volver BRANCH un edge q ya esta E_REJECTED ");
              break;
            }

        }
    }

    if(e_aux == NULL)
    {
        MY_DBG_1("ERROR: no se encontro vecino %d en la lista de edges (become_branch)\n"
               ,node_addr->u8[0]);
    }

    //Si se vuelve alguien branch toca evaluar los mensajes de connect.
    //Porque en el texto dice  "qp is or becomes a branch edge" pg. 83 del PDF
    process_post(&evaluar_msg_co, PROCESS_EVENT_CONTINUE, NULL); // "qp is or becomes a branch edge"
}

/* Funcion que devuelve el numero de hijos
*/
uint8_t num_hijos(edges *e_list_head)
{
    uint8_t numero_hijos = 0;

    edges *e_aux = NULL;
    for(e_aux = e_list_head; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
    {
        if(e_aux->state == BRANCH) //Entra si las direcciones son iguales
        {
            numero_hijos = numero_hijos + 1;
        }
    }

    if(nd.flags & CORE_NODE) //pregunto si soy CORE_NODE
    {
        return (numero_hijos); //aca el padre (el otro core_node) tambien es hijo al mismo tiempo
    }else
    {
        return (numero_hijos - 1); //menos la branch del padre
    }
}

/* Devuelve un apuntador al basic edge (su addr) que tenga menor peso.
* Least_basic_edge = Encuentra el basic edge de menor peso.
* (Lista ya ordenada en master_neighbor_discovery)
*/
linkaddr_t* least_basic_edge(edges *e_list_head)
{
    edges *e_aux = NULL;
    for(e_aux = e_list_head; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
    {
        //tener en cuenta que la lista de edges ya esta
        //ordenada de menor a mayor desde el proceso
        //master_neighbor_discovery. Entonces solo necesito
        //evaluar que el edge sea basic
        if(e_aux->state == BASIC)
        {
            break;
        }
    }

    if( e_aux == NULL )
    {
        MY_DBG_1("ERROR: No existe ningun basic edge\n");
        return NULL;
    }else
    {
        return &e_aux->addr;
    }
}

/*Funcion para retornar el peso del edge
*/
uint32_t weight_with_edge(const linkaddr_t *addr,  edges *e_list_head)
{
    edges *e_aux = NULL;
    for(e_aux = e_list_head; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
    {
        if(linkaddr_cmp(addr, &e_aux->addr )) //Entra si las direcciones son iguales
        {
            break;
        }
    }

    if(e_aux == NULL)
    {
        MY_DBG_1("ERROR: El vecino no existe dentro de la lista de edges\n");
        return INFINITO;
    }else
    {
        return (e_aux->weight);
    }
}
/*Funcion para saber si el estado de un edge es branch. Se busca por addr
*/
uint8_t state_is_branch(const linkaddr_t *addr,  edges *e_list_head)
{
    edges *e_aux = NULL;
    for(e_aux = e_list_head; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
    {
        if(linkaddr_cmp(addr, &e_aux->addr )) //Entra si las direcciones son iguales
        {
            break;
        }
    }
    if (e_aux == NULL)
    {
        MY_DBG_1("ERROR: El nodo (addr) no esta en mi lista de edges (vecinos)");
        return 0; //Digo q no es branch. Ni siquiera es vecino
    }else
    {
        if(e_aux->state == BRANCH)
        {
            return 1;
        }else
        {
            return 0;
        }
    }
}

// Funcion para llenar el nombre del fragmento
// Este nombre esta compuesto de WeightEntre2Nodos, address del core_node 1, address del core_node 2
void llenar_name_str(name *name_str, uint32_t weight, linkaddr_t *core_node_2)
{
    //core_node_1 es la direccion menor
    //core_node_2 es la direccion mayor
    if( core_node_2->u8[0] >= linkaddr_node_addr.u8[0])
    {
        linkaddr_copy(&name_str->core_node_1, &linkaddr_node_addr);
        linkaddr_copy(&name_str->core_node_2, core_node_2);
    }
    else
    {
        linkaddr_copy(&name_str->core_node_1, core_node_2);
        linkaddr_copy(&name_str->core_node_2, &linkaddr_node_addr);
    }

    name_str->weight = weight;

}
/* Hace la inicializacion del proceso master_co_i
*/
void init_master_co_i(struct neighbor *n_list_head, struct memb *edges_memb, list_t edges_list)
{
    MY_DBG_3("Process Init: master_co_i \n");

    //Variables locales
    linkaddr_t *lwoe_init; //LWOE inicial. Es el edge con menor weight
    char string[] = "READ";

    ////////////////////////////////////////////////
    /////////////REINICIAR VARIABLE/////////////////
    ////////////////////////////////////////////////

    //Inicializacion de "struct node"
    nd.state = FOUND;   //Inicio en FOUND porque ya se que el Basic edge con menor peso es el LWOE
    MY_DBG_3("Estoy en FOUND virtual \n"); //virtualmente porque no quiero resetear ND_LWOE ni CH_LWOE
    nd.flags = 0;
    llenar_name_str(&nd.f.name_str, 0, &linkaddr_node_addr); //Inicio weight con 0
    nd.f.level = 0; //Inicio level con 0
    linkaddr_copy(&nd.parent, &linkaddr_node_addr); //yo mismo soy mi padre
    nd.lwoe.node.weight = INFINITO;
    linkaddr_copy(&nd.lwoe.node.neighbor, &linkaddr_node_addr); //si peso es infinito, yo soy vecino
    nd.lwoe.children.weight = INFINITO;
    linkaddr_copy(&nd.lwoe.children.neighbor, &linkaddr_node_addr); //si peso es infinito, yo soy vecino
    linkaddr_copy(&nd.downroute, &linkaddr_node_addr); //yo mismo soy downroute
    linkaddr_copy(&nd.otro_core_node, &linkaddr_node_addr); //otro core node soy YO

    //Tomar info de master_neighbor_discovery
    fill_edges_list(edges_list, edges_memb, n_list_head );

    // llenar la variable global con la cabeza de la lista
    e_list_head_g = list_head(edges_list);

    // Devuelve el basic edge con menor peso
    lwoe_init = least_basic_edge(list_head(edges_list));

    //Setear LWOE del nodo
    linkaddr_copy(&nd.lwoe.node.neighbor, lwoe_init);
    nd.lwoe.node.weight = weight_with_edge(lwoe_init, list_head(edges_list));

    // Volver el basic edge con menor peso branch
    become_branch(list_head(edges_list),  &nd.lwoe.node.neighbor ); //become branch inicial level = 0
    MY_DBG_3("primer become branch nodo = %d  \n", nd.lwoe.node.neighbor.u8[0]);

    //imprimir la info que tome de fill_edges_list y guarde en edges_list
    print_edges_list(list_head(edges_list), string, &linkaddr_node_addr);

}
/* LLena un msg de initiate con los valores parametros
*/
void llenar_initiate_msg(initiate_msg *i_msg, name name_str,
                        uint8_t level, uint8_t state, const linkaddr_t *dest, uint8_t flags)
{
    i_msg->f.name_str = name_str;
    i_msg->f.level    = level;
    i_msg->nd_state   = state;
    linkaddr_copy(&i_msg->destination , dest);
    i_msg->flags = flags;
}

// llenar la lista de msg de initiate
void llenar_initiate_msg_list (initiate_list *i_list_out_p, name name_str,
                        uint8_t level, uint8_t state, const linkaddr_t *dest, uint8_t flags)
{
    i_list_out_p->i_msg.f.name_str = name_str;
    i_list_out_p->i_msg.f.level    = level;
    i_list_out_p->i_msg.nd_state   = state;
    linkaddr_copy(&i_list_out_p->i_msg.destination , dest);
    i_list_out_p->i_msg.flags = flags;
}


/* LLena un msg de connect con los valores parametros
*/
void llenar_connect_msg (connect_msg *msg, uint8_t level, linkaddr_t *destination)
{
    msg->level = level;
    linkaddr_copy(&msg->destination,  destination);
}

void llenar_connect_msg_list (connect_list *co_list_out_p, uint8_t level, linkaddr_t *destination)
{
    co_list_out_p->co_msg.level = level;
    linkaddr_copy(&co_list_out_p->co_msg.destination,  destination);
}

// funcion para que el nodo haga lo que tiene que hacer al volverse core_node
void become_core_node(linkaddr_t *otro_core_node)
{
    nd.flags |= CORE_NODE; //seteo la bandera de ser core_node
    linkaddr_copy(&nd.otro_core_node, otro_core_node); //guardo quien es el otro core_node
    MY_DBG_3("Soy CORE_NORE\n");
}

// funcion para que el nodo haga lo que tiene que hacer al dejar de ser core_node
void stop_being_core_node()
{
    nd.flags &= ~CORE_NODE; //desSeteo la bandera de ser core_node
    linkaddr_copy(&nd.otro_core_node, &linkaddr_node_addr); //otro core node soy YO
    MY_DBG_3("Dejo de ser CORE_NORE\n");
}
