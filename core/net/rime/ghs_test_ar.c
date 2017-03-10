/*-------------------------------------------------------------------*/
/*---------------- INCLUDES -----------------------------------------*/
/*-------------------------------------------------------------------*/

#include <stdio.h>
#include "ghs_test_ar.h"

/*-------------------------------------------------------------------*/
/*---------------- FUNCIONES ----------------------------------------*/
/*-------------------------------------------------------------------*/
void init_master_test_ar(struct process *master_co_i, struct process *send_message_test_ar,
                         struct process *e_pospone_test)
{
    printf("Process Init: master_test_ar \n");

    //Terminar procesos
    process_exit(master_co_i);   //Cierro el proceso anterior para liberar memoria

    //Iniciar procesos nuevos
    process_start(send_message_test_ar, NULL);
    process_start(e_pospone_test, NULL);



}
