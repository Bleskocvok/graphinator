#pragma once

#ifndef __CPU_INFO_H__
#define __CPU_INFO_H__


typedef enum
{
    USER, NICE, SYSTEM, IDLE, IOWAIT, IRQ, SOFTIRQ, STEAL, GUEST, GUEST_NICE,
    PROC_STAT_TYPE_COUNT

} proc_stat_type_t;


typedef struct
{
    unsigned long long fields[ PROC_STAT_TYPE_COUNT ];

} proc_stat_t;


int proc_stat_read_str( proc_stat_t* stat, const char* str );
int proc_stat_read( proc_stat_t* buf, int bufsize );

unsigned long long proc_stat_total( proc_stat_t* stat );
unsigned long long proc_stat_idle( proc_stat_t* stat );
unsigned long long proc_stat_work( proc_stat_t* stat );


#endif // __CPU_INFO_H__
