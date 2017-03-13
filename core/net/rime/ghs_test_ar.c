/*-------------------------------------------------------------------*/
/*---------------- INCLUDES -----------------------------------------*/
/*-------------------------------------------------------------------*/

#include <stdio.h>
#include "ghs_test_ar.h"




/*-------------------------------------------------------------------*/
/*---------------- FUNCIONES ----------------------------------------*/
/*-------------------------------------------------------------------*/

/* Funcion para inicializar el proceso master_test_ar
*/
void init_master_test_ar(struct process *master_co_i, struct process *send_message_test_ar,
                         struct process *e_pospone_test, struct process *e_test)
{
    //printf("Process Init: master_test_ar \n");

    //Terminar procesos
    process_exit(master_co_i);   //Cierro el proceso anterior para liberar memoria

    //Iniciar procesos nuevos
    process_start(send_message_test_ar, NULL);
    process_start(e_pospone_test, NULL);
    process_start(e_test, NULL);



}

/* Funcion para llenar el msg test
*/
void llenar_test_msg (test_msg *t_msg, const linkaddr_t *destination, fragment f)
{
    linkaddr_copy(&t_msg->destination,  destination);
    t_msg->f.name      = f.name;
    t_msg->f.level     = f.level;
}
