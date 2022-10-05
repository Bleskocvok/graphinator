#pragma once

#ifndef __COLLECTOR_H__
#define __COLLECTOR_H__


typedef double ( *collect_func_t ) ( void* );
typedef void*  ( *init_func_t )    ( void  );
typedef void   ( *free_func_t )    ( void* );


typedef struct
{
    init_func_t init;
    collect_func_t collect;
    free_func_t free;

    void* ptr;

} collector_t;


void collector_init( collector_t* collector );
void collector_reset( collector_t* collector, init_func_t init,
                      collect_func_t collect, free_func_t free );
void collector_free( collector_t* collector );

double collector_collect( collector_t* collector );


void* init_cpu_data( void );
void free_cpu_data( void* ptr );
double collect_cpu_data( void* ptr );


void* init_mem_data( void );
void free_mem_data( void* ptr );
double collect_mem_data( void* ptr );


#endif // __COLLECTOR_H__
