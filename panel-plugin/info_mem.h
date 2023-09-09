#pragma once

#ifndef __INFO_MEM_H__
#define __INFO_MEM_H__


#include <stddef.h>         // size_t


typedef struct
{
    unsigned long total,
                  avail,
                  free;

    char* buf;
    size_t allocated;

} proc_mem_t;


int proc_mem_init( proc_mem_t* info );
void proc_mem_free( proc_mem_t* info );
int proc_mem_read( proc_mem_t* info );


#endif // __INFO_MEM_H__
