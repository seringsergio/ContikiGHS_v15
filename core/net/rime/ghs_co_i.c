/*-------------------------------------------------------------------*/
/*---------------- INCLUDES -----------------------------------------*/
/*-------------------------------------------------------------------*/
#include "ghs_co_i.h"
/*-------------------------------------------------------------------*/
/*---------------- FUNCIONES-----------------------------------------*/
/*-------------------------------------------------------------------*/

node nd; //nd es node....n es neighbor

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

        e -> state  = BASIC; //Todos los edges inician con estado BASIC
        linkaddr_copy(&e->addr,  &n_aux->addr);
        e -> weight = n_aux -> avg_seqno_gap;

        list_add(edges_list, e); //Agregarlo a la lista
    }

}

/* Imprime la lista de edges
*/
void print_edges_list(edges *e_list_head, char *string,  const linkaddr_t *node_addr)
{
    edges *e_aux = NULL;

    for(e_aux = e_list_head; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
    {
        printf("%s %d %d %d.%02d %d\n",
              string,
              node_addr->u8[0],
              e_aux->addr.u8[0],
              (int)(e_aux->weight / SEQNO_EWMA_UNITY),
              (int)(((100UL * e_aux->weight) / SEQNO_EWMA_UNITY) % 100),
              e_aux->state);
    }
}

/* Un edge pasa de estado BASIC a BRANCH.
*  Become_branch = Vuelve branch un edge
*/
void become_branch(edges *e_list_head, const linkaddr_t *node_addr)
{
    edges *e_aux;
    for(e_aux = e_list_head; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
    {
        if(linkaddr_cmp(&e_aux->addr, node_addr)) //Entra si las direcciones son iguales
        {
            e_aux->state = BRANCH;
            break;
        }
    }
}

/* Devuelve un apuntador al basic edge (su addr) que tenga menor peso.
* Least_basic_edge = Encuentra el basic edge de menor peso.
* (Lista ya ordenada en master_neighbor_discovery)
*/
linkaddr_t* least_basic_edge(edges *e_list_head)
{
    edges *e_aux;
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
    return &e_aux->addr;
}
/* Es la funcion que se llama cuando hay un timeout en el runicast
*/
void ghs_co_i_timedout_ruc(const linkaddr_t *to, uint8_t retransmissions)
{
    printf("runicast message timed out when sending to %d.%d, retransmissions %d\n",
  	 to->u8[0], to->u8[1], retransmissions);
}
/* Send un mensaje de runicast
*/
void ghs_co_i_send_ruc(const linkaddr_t *to, uint8_t retransmissions)
{
    /*printf("runicast message sent to %d.%d, retransmissions %d\n",
       to->u8[0], to->u8[1], retransmissions);*/
}

/*Funcion para retornar el peso del edge
*/
uint32_t weight_with_edge(const linkaddr_t *addr,  edges *e_list_head)
{
    edges *e_aux;
    for(e_aux = e_list_head; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
    {
        if(linkaddr_cmp(addr, &e_aux->addr )) //Entra si las direcciones son iguales
        {
            break;
        }
    }
    return (e_aux->weight);
}
/*Funcion para saber si el estado de un edge es branch. Se busca por addr
*/
uint8_t state_is_branch(const linkaddr_t *addr,  edges *e_list_head)
{
    edges *e_aux;
    for(e_aux = e_list_head; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
    {
        if(linkaddr_cmp(addr, &e_aux->addr )) //Entra si las direcciones son iguales
        {
            break;
        }
    }
    if(e_aux->state == BRANCH)
    {
        return 1;
    }else
    {
        return 0;
    }
}

/* Hace la inicializacion del proceso master_co_i
*/
void init_master_co_i(struct neighbor *n_list_head, struct process *master_neighbor_discovery,
                        struct process *send_message_co_i, struct process *e_pospone_connect ,
                        struct memb *edges_memb, list_t edges_list,
                        struct process *reports_completos, struct process *e_LWOE,
                        struct process *send_message_report_ChaRoot, struct process *e_test,
                        struct process *send_message_test_ar, struct process *e_pospone_test)
{
    printf("Process Init: master_co_i \n");

    //Variables locales
    linkaddr_t *lwoe_init; //LWOE inicial. Es el edge con menor weight
    char string[] = "READ";

    //Inicializacion de Variables globales
    nd.flags = 0;
    nd.f.name = 0;
    nd.f.level = 0;
    nd.num_children = 0;

    //Terminar procesos
    process_exit(master_neighbor_discovery);   //Se cierra el proceso y se llama el PROCESS_EXITHANDLER(funcion)


    //Iniciar procesos nuevos
    process_start(send_message_co_i, NULL);
    process_start(e_pospone_connect, NULL);

    //Procesos de test ar
    process_start(send_message_test_ar, NULL);
    process_start(e_pospone_test, NULL);
    process_start(e_test, NULL);

    //procesos de report-ChangeRoot
    process_start(reports_completos, NULL); //para inicializar report_list_g y report_memb_g
    process_start(send_message_report_ChaRoot, NULL); //para inicializar report_list_g y report_memb_g
    process_start(e_LWOE, NULL); //para inicializar report_list_g y report_memb_g

    //Tomar info de master_neighbor_discovery
    fill_edges_list(edges_list, edges_memb, n_list_head );

    // llenar la variable global con la cabeza de la lista
    e_list_head_g = list_head(edges_list);

    // Vuelve Branch el basic edge con menor peso
    lwoe_init = least_basic_edge(list_head(edges_list));
    become_branch(list_head(edges_list),  lwoe_init ); //become branch inicial level = 0

    //Setear LWOE del nodo
    linkaddr_copy(&nd.lwoe.node.neighbor, lwoe_init);
    nd.lwoe.node.weight = return_weight( list_head(edges_list), lwoe_init);
    /*nd.flags |= ND_LWOE; //Ya encontre el ND_LWOE
    process_post(e_LWOE, PROCESS_EVENT_CONTINUE, NULL);*/

    //imprimir la info que tome de fill_edges_list y guarde en edges_list
    print_edges_list(list_head(edges_list), string, &linkaddr_node_addr);

}
/* LLena un msg de initiate con los valores parametros
*/
void llenar_initiate_msg(initiate_msg *i_msg, uint32_t name,
                        uint8_t level, uint8_t state, const linkaddr_t *dest, uint8_t flags)
{
    i_msg->f.name     = name;
    i_msg->f.level    = level;
    i_msg->nd_state   = state;
    linkaddr_copy(&i_msg->destination , dest);

    if( !(flags & BECOME_CORE_NODE))
    {
        i_msg->flags     &= ~BECOME_CORE_NODE;
    }else
    if(flags & BECOME_CORE_NODE)
    {
        i_msg->flags     |= BECOME_CORE_NODE;
    }
}
/* LLena un msg de connect con los valores parametros
*/
void llenar_connect_msg (connect_msg *msg, uint8_t level, linkaddr_t *destination)
{
    msg->level = level;
    linkaddr_copy(&msg->destination,  destination);
}

/* LLena un msg de pospone connect con los valores parametros
*/
void llenar_pospone_connect(pospone_connect *pc, const linkaddr_t *neighbor, connect_msg co_msg)
{
    linkaddr_copy(&pc->neighbor, neighbor);
    pc->co_msg = co_msg;
}


/*---------------------------------------------------------------------------*/
/* Funcion que recibe un mensaje de runicast: Guarda en history_list los vecinos que
* han enviado msg y su seq. Si el avg_seqno_gap del vecino es
*  mayor, entonces reemplazo mi avg_seqno_gap.
*/
void ghs_co_i_recv_ruc(void *msg, const linkaddr_t *from,
                    struct memb *history_mem, list_t history_list, uint8_t seqno,
                     edges *e_list_head, struct process *send_message_co_i,
                    struct memb *pc_memb  ,list_t pc_list, struct process *master_co_i,
                    struct process *e_pospone_connect, struct process *e_pospone_test)
{
    // OPTIONAL: Sender history
    struct history_entry *e = NULL;
    uint8_t duplicate = 0;

    for(e = list_head(history_list); e != NULL; e = e->next) {
      if(linkaddr_cmp(&e->addr, from)) { // Si las dir son iguales entra
        break;
      }
    }
    if(e == NULL) {
      // Create new history entry
      e = memb_alloc(history_mem);
      if(e == NULL) {
        e = list_chop(history_list); /* Remove oldest at full history */
      }
      linkaddr_copy(&e->addr, from);
      e->seq = seqno;
      list_push(history_list, e);
    } else {
      // Detect duplicate callback
      if(e->seq == seqno) {
        duplicate = 1;
        printf("runicast message received from %d.%d, seqno %d (DUPLICATE)\n",
  	     from->u8[0], from->u8[1], seqno);
        return;
      }
      // Update existing history entry
      e->seq = seqno;
    }

    /*printf("runicast message received from %d.%d, seqno %d\n",
  	 from->u8[0], from->u8[1],
       seqno);*/

   if(duplicate == 0)
   {
        //Leer el packet buffer attribute: Especificamente el tipo de mensaje
        packetbuf_attr_t msg_type = packetbuf_attr(PACKETBUF_ATTR_PACKET_GHS_TYPE_MSG);

        // Evaluo el tipo de msg que llego
        if(msg_type == CONNECT)
        {
            initiate_msg i_msg;
            connect_msg *co_msg = (connect_msg *) msg;

            printf("LLEga ConNect de %d con level=%d \n", from->u8[0], co_msg->level);
            if(co_msg->level == nd.f.level) //Si los dos fragmentos tienen el mismo nivel
            {
                if(state_is_branch(from, e_list_head)) // Caso inicial. Fragmentos con 1 nodo
                {
                    nd.num_children = nd.num_children + 1;
                    nd.flags |= CORE_NODE;

                    llenar_initiate_msg(&i_msg, weight_with_edge(from, e_list_head),
                                        (nd.f.level + 1), FIND, from, BECOME_CORE_NODE);
                    process_post(send_message_co_i,  e_msg_initiate, &i_msg); //Hijo + 1 !!
                    printf("Soy CORE_NORE 3\n");

                }else //Si el estado NO es branch (El proceso postpones processing CONECT)
                {
                    pospone_connect *pc_aux = NULL;
                    for(pc_aux = list_head(pc_list); pc_aux != NULL; pc_aux = list_item_next(pc_aux)) // Recorrer toda la lista
                    {
                        if(linkaddr_cmp(&pc_aux->neighbor, from)) { // Si las dir son iguales entra
                          break;
                        }
                    }
                    if(pc_aux == NULL) //SI no existe un pospone para el nodo
                    {
                        // Create new history entry
                        pc_aux = memb_alloc(pc_memb);
                        if(pc_aux == NULL)
                        {
                            printf("ERROR: NO PUDE CREAR UNA ENTRADA PARA POSPONE CONNECT \n");
                        }else
                        {
                            llenar_pospone_connect(pc_aux, from, *co_msg);
                            list_push(pc_list, pc_aux); //Add an item to the start of the list.
                            printf("Agregado CONECT POSPONE de  %d \n", pc_aux->neighbor.u8[0]);
                        }
                    }else //Existe un pospone connect para el nodo, pero lo update
                    {
                        printf("Llegaron 2 mensajes de CONNECT POSPONE del nodo %d \n"
                              ,from->u8[0]);
                        //Reemplazo (update) los valores del mensaje de connect
                        llenar_pospone_connect(pc_aux, from, *co_msg);
                    }
                } //FIN de pospone connect
            }else
            if(co_msg->level < nd.f.level)
            {
                become_branch(e_list_head, from); // become branch de connect
                nd.num_children = nd.num_children + 1;
                llenar_initiate_msg(&i_msg, nd.f.name,nd.f.level, nd.state, from, ~BECOME_CORE_NODE);
                process_post(send_message_co_i,  e_msg_initiate, &i_msg); //Hijo + 1 !!
                printf("Soy mayor \n");
            }
            /*printf("llego CONNECT from %d.%d con level = %d\n",
                  from->u8[0], from->u8[1],
                  co_msg->level);*/
        }else
        if(msg_type == INITIATE)
        {
            initiate_msg *i_msg = (initiate_msg *) msg;
            initiate_msg i_msg_d;

            nd.f.name  = i_msg->f.name;
            nd.f.level = i_msg->f.level;
            nd.state   = i_msg->nd_state;
            linkaddr_copy(&nd.parent , from);

            if(i_msg->flags & BECOME_CORE_NODE)
            {
                nd.flags |= CORE_NODE;
                printf("Soy CORE_NORE 2\n");
            }

            if(i_msg->nd_state == FIND) //si cambio de estado a FIND
            {
                //Envio un mensaje al master_co_i de find
                process_post(master_co_i,  e_find, NULL);
                nd.state = FIND;  //Para saber en que estado estoy en cualquier parte
            }

            //Reenvio el msg por todas las BRANCHES
            edges *e_aux;
            for(e_aux = e_list_head; e_aux != NULL; e_aux = list_item_next(e_aux)) // Recorrer toda la lista
            {
                //Propagar el INITIATE por las otras ramas
                //Si es una BRANCH y no es el nodo que me envio el INITIATE (No le devuelvo el msg)
                if( (e_aux->state == BRANCH) && !linkaddr_cmp(&e_aux->addr, from))
                {
                    //nd.num_children = nd.num_children + 1;
                    llenar_initiate_msg(&i_msg_d, i_msg->f.name,i_msg->f.level,
                                       i_msg->nd_state, &e_aux->addr, ~BECOME_CORE_NODE);
                    process_post(send_message_co_i,  e_msg_initiate, &i_msg_d);
                }
            }

            printf("llego INITIATE from %d.%d name=%d.%02d level=%d state=%d parent=%d\n",
                  from->u8[0], from->u8[1],
                  (int)(nd.f.name / SEQNO_EWMA_UNITY),
                  (int)(((100UL * nd.f.name) / SEQNO_EWMA_UNITY) % 100),
                  nd.f.level,
                  nd.state,
                  nd.parent.u8[0]);
        } //END if msg es INITIATE

        //LLamar al proceso para que evalue el pospone agregado o actualizado
        //Esto es equivalente a mandar el pc al final de la cola
        process_post(e_pospone_connect, PROCESS_EVENT_CONTINUE, NULL ) ;
        process_post(e_pospone_test, PROCESS_EVENT_CONTINUE, NULL ) ;



    } //END of IF duplicate == 0
} //END runicas receive
