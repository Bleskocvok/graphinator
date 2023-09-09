#pragma once

#ifndef __MEM_INFO_H__
#define __MEM_INFO_H__


typedef struct
{
    unsigned long total,
                  avail,
                  free;

} proc_mem_t;


int proc_mem_read( proc_mem_t* info );


#endif // __MEM_INFO_H__
