#pragma once

#ifndef __COLLECTOR_CPU_H__
#define __COLLECTOR_CPU_H__


#include "collector.h"


#define  CPU_COLLECTOR_INIT  (collector_t) { .collect = collect_cpu_data, \
                                             .init = init_cpu_data,       \
                                             .free = free_cpu_data,       \
                                             .get_unit = get_percent,     \
                                             .max_value = 100,            }

extern const collector_t cpu_collector;


void* init_cpu_data( void );
void free_cpu_data( void* ptr );
double collect_cpu_data( void* ptr );


#endif
