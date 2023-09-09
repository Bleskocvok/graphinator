#pragma once

#ifndef __COLLECTOR_MEM_H__
#define __COLLECTOR_MEM_H__


#include "collector.h"


#define  MEM_COLLECTOR_INIT  (collector_t) { .collect = collect_mem_data, \
                                             .init = init_mem_data,       \
                                             .free = free_mem_data,       \
                                             .get_unit = get_percent,     \
                                             .max_value = 100,            }


extern const collector_t mem_collector;


void* init_mem_data( void );
void free_mem_data( void* ptr );
double collect_mem_data( void* ptr );


#endif
