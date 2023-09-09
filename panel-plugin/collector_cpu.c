
#include "collector_cpu.h"

#include "info_cpu.h"

#include <stddef.h>         // NULL
#include <stdlib.h>         // calloc, free


const collector_t cpu_collector = CPU_COLLECTOR_INIT;


void* init_cpu_data( void )
{
    proc_stat_t* ptr = (proc_stat_t*) calloc( 1, sizeof( proc_stat_t ) );
    if ( !ptr )
        return NULL;

    // TODO: check return value
    proc_stat_init( ptr );

    proc_stat_read( ptr, 1 );
    return ptr;
}


void free_cpu_data( void* ptr )
{
    proc_stat_free( (proc_stat_t*) ptr );
    free( ptr );
}


double collect_cpu_data( void* ptr )
{
    proc_stat_t* prev_stat = ptr;

    proc_stat_t stat = { 0 };

    proc_stat_read( &stat, 1 );

    unsigned long long total =  ( proc_stat_total( &stat )
                                - proc_stat_total( prev_stat ) );

    double frac = total == 0 ? 0  // <-- avoid division by zero
                             : ( (double) ( proc_stat_work( &stat )
                                          - proc_stat_work( prev_stat ) )
                               / (double) total );

    *prev_stat = stat;

    return frac * 100;
}
