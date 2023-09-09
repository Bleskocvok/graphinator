
#include "collector_mem.h"

#include "mem_info.h"

#include <stddef.h>         // NULL


const collector_t mem_collector = MEM_COLLECTOR_INIT;


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
