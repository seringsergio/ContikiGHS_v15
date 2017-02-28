/*-------------------------------------------------------------------*/
/*---------------- INCLUDES -----------------------------------------*/
/*-------------------------------------------------------------------*/
#include <stdio.h>
#include "ghs.h"

/*-------------------------------------------------------------------*/
/*---------------- FUNCIONES ----------------------------------------*/
/*-------------------------------------------------------------------*/
void copy_data( struct neighbor *dest, struct neighbor *source  )
{
 linkaddr_copy(&dest->addr,  &source->addr);
 dest->last_rssi     = source->last_rssi;
 dest->last_lqi      = source->last_lqi;
 dest->last_seqno    = source->last_seqno;
 dest->avg_seqno_gap = source->avg_seqno_gap;
}
