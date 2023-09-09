#pragma once

#ifndef __COLLECTOR_H__
#define __COLLECTOR_H__


typedef double      ( *collect_func_t )     ( void* );
typedef void*       ( *init_func_t )        ( void  );
typedef void        ( *free_func_t )        ( void* );
typedef const char* ( *get_unit_func_t )    ( void* );


typedef struct
{
    init_func_t init;
    free_func_t free;
    collect_func_t collect;
    get_unit_func_t get_unit;

    void* ptr;

    double max_value;

} collector_t;


void collector_init( collector_t* collector );
void collector_reset( collector_t* collector, collector_t* new_c );
void collector_free( collector_t* collector );
double collector_collect( collector_t* collector );
int collector_equals( const collector_t* a, const collector_t* b );
const char* collector_get_unit( const collector_t* collector );


const char* get_percent( void* );


#endif // __COLLECTOR_H__
