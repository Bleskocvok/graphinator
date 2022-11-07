#pragma once

#ifndef __COLLECTOR_H__
#define __COLLECTOR_H__


#define  CPU_COLLECTOR_INIT  (collector_t) { .collect = collect_cpu_data, \
                                             .init = init_cpu_data,       \
                                             .free = free_cpu_data,       \
                                             .get_unit = get_percent,     \
                                             .max_value = 100,            }

#define  MEM_COLLECTOR_INIT  (collector_t) { .collect = collect_mem_data, \
                                             .init = init_mem_data,       \
                                             .free = free_mem_data,       \
                                             .get_unit = get_percent,     \
                                             .max_value = 100,            }


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


extern const collector_t cpu_collector;
extern const collector_t mem_collector;


void collector_init( collector_t* collector );
void collector_reset( collector_t* collector, collector_t* new_c );
void collector_free( collector_t* collector );
double collector_collect( collector_t* collector );
const char* collector_get_unit( const collector_t* collector );


const char* get_percent( void* );

void* init_cpu_data( void );
void free_cpu_data( void* ptr );
double collect_cpu_data( void* ptr );


void* init_mem_data( void );
void free_mem_data( void* ptr );
double collect_mem_data( void* ptr );


#endif // __COLLECTOR_H__
