
#include "collector.h"

#include <stdlib.h>         // NULL


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


int collector_equals( const collector_t* a, const collector_t* b )
{
    return a->collect == b->collect;
}


const char* get_percent( void* ptr )
{
    (void) ptr;
    return "%%";
}
