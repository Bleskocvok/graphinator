
#include "collector_mem.h"

#include "info_mem.h"

#include <stddef.h>         // NULL
#include <stdlib.h>         // calloc, free


const collector_t mem_collector = MEM_COLLECTOR_INIT;


void* init_mem_data( void )
{
    proc_mem_t* ptr = (proc_mem_t*) calloc( 1, sizeof( proc_mem_t ) );
    if ( !ptr )
        return NULL;

    // TODO: check return value
    proc_mem_init( ptr );

    proc_mem_read( ptr );
    return ptr;

}


void free_mem_data( void* ptr )
{
    proc_mem_free( (proc_mem_t*) ptr );
}


double collect_mem_data( void* ptr )
{
    proc_mem_t* info = ptr;
    proc_mem_read( info );
    if ( info->total == 0 )
        return -1;
    return ( info->total - info->avail ) / (double) info->total * 100;
}
