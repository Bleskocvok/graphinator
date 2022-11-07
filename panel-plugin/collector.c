
#include "collector.h"

#include "cpu_info.h"
#include "mem_info.h"

#include <stdlib.h>         // NULL


const collector_t cpu_collector = CPU_COLLECTOR_INIT;
const collector_t mem_collector = MEM_COLLECTOR_INIT;


void collector_init( collector_t* collector )
{
    if ( collector->init )
        collector->ptr = collector->init();
}


void collector_reset( collector_t* collector, collector_t* new_c )
{
    if ( collector->ptr )
        collector_free( collector );

    *collector = *new_c;

    if ( !collector->ptr )
        collector_init( collector );
}


void collector_free( collector_t* collector )
{
    if ( collector->free )
    {
        collector->free( collector->ptr );
        collector->ptr = NULL;
    }
}


double collector_collect( collector_t* collector )
{
    return collector->collect( collector->ptr );
}


const char* collector_get_unit( const collector_t* collector )
{
    if ( collector->get_unit )
        return collector->get_unit( collector->ptr );
    return "";
}


const char* get_percent( void* ptr )
{
    (void) ptr;
    return "%%";
}


void* init_mem_data( void )
{
    return NULL;
}


void free_mem_data( void* ptr )
{
    (void) ptr;
}


double collect_mem_data( void* ptr )
{
    (void) ptr;

    proc_mem_t info;
    proc_mem_read( &info );
    if ( info.total == 0 )
        return -1;
    return ( info.total - info.avail ) / (double) info.total * 100;
}


void* init_cpu_data( void )
{
    proc_stat_t* ptr = calloc( 1, sizeof( proc_stat_t ) );
    if ( !ptr )
        return NULL;

    proc_stat_read( ptr, 1 );
    return ptr;
}


void free_cpu_data( void* ptr )
{
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

