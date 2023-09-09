
// getline
#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

// custom
#include "info_cpu.h"

// c
#include <stdlib.h>         // NULL, free, strtoull
#include <stdio.h>          // FILE, fopen, fclose, feof, ferror
#include <string.h>         // strncmp
#include <ctype.h>          // isdigit, isspace

// posix
#include <stdio.h>          // getline


int proc_stat_init( proc_stat_t* stat )
{
    *stat = (proc_stat_t){ 0 };
    return 0;
}


void proc_stat_free( proc_stat_t* stat )
{
    free( stat->buf );
}


int proc_stat_read_str( proc_stat_t* stat, const char* str )
{
    int i = 0;
    char* end;

    for ( ; i < PROC_STAT_TYPE_COUNT; ++i )
    {
        // skip whitespace
        while ( isspace( str[ 0 ] ) )
            ++str;

        // parse number
        stat->fields[ i ] = strtoull( str, &end, 10 );
        if ( end == str )
            break;
        str = end;
    }
    return i;
}


int proc_stat_read( proc_stat_t* stat, int stat_count )
{
    stat->file = fopen( "/proc/stat", "r" );

    if ( ferror( stat->file ) )
        return fclose( stat->file ), stat->file = NULL, 0;

    if ( !stat->file )
        return 0;

    int i = 0;

    for ( ; !feof( stat->file ) && i < stat_count; ++i )
    {
        // TODO: reuse this buffer for all polls
        // -1 cause of \n
        ssize_t len = getline( &stat->buf, &stat->allocated, stat->file ) - 1;
        // remove newline
        stat->buf[ len ] = '\0';

        char* line = stat->buf;

        if ( strncmp( line, "cpu", 3 ) != 0 )
            break;

        line += 3;

        // skip cpu digits
        while ( len > 0 && isdigit( line[ 0 ] ) )
            ++line;

        proc_stat_read_str( stat + i, line );
    }

    fclose( stat->file );

    return i;
}


unsigned long long proc_stat_total( proc_stat_t* stat )
{
    unsigned long long res = 0;
    for ( int i = 0; i < PROC_STAT_TYPE_COUNT; ++i )
        res += stat->fields[ i ];
    return res;
}


unsigned long long proc_stat_idle( proc_stat_t* stat )
{
    return proc_stat_total( stat ) - proc_stat_work( stat );
}


unsigned long long proc_stat_work( proc_stat_t* stat )
{
    return stat->fields[ USER ]
         + stat->fields[ NICE ]
         + stat->fields[ SYSTEM ]
         // not entirely sure about these following two, whether or not they
         // count as the processor doing work
         + stat->fields[ GUEST ]
         + stat->fields[ GUEST_NICE ];
}
