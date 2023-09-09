#pragma once

#ifndef __INFO_CPU_H__
#define __INFO_CPU_H__


#include <stdio.h>      // FILE


typedef enum
{
    USER, NICE, SYSTEM, IDLE, IOWAIT, IRQ, SOFTIRQ, STEAL, GUEST, GUEST_NICE,
    PROC_STAT_TYPE_COUNT

} proc_stat_type_t;


typedef struct
{
    unsigned long long fields[ PROC_STAT_TYPE_COUNT ];
    FILE* file;
    char* buf;
    size_t allocated;

} proc_stat_t;


int proc_stat_init( proc_stat_t* stat );
void proc_stat_free( proc_stat_t* stat );

int proc_stat_read_str( proc_stat_t* stat, const char* str );
int proc_stat_read( proc_stat_t* buf, int bufsize );

unsigned long long proc_stat_total( proc_stat_t* stat );
unsigned long long proc_stat_idle( proc_stat_t* stat );
unsigned long long proc_stat_work( proc_stat_t* stat );


#endif // __INFO_CPU_H__
