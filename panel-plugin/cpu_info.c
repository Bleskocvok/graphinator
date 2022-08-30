
// getline
#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

// custom
#include "cpu_info.h"

// c
#include <stdlib.h>         // NULL, free, strtoull
#include <stdio.h>          // FILE, fopen, fclose, feof, ferror
#include <string.h>         // strncmp
#include <ctype.h>          // isdigit, isspace

// posix
#include <stdio.h>          // getline


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
    static const char* cpu_info_filename = "/proc/stat";

    FILE* file = fopen( cpu_info_filename, "r" );

    char* buf = NULL;
    size_t allocated = 0;

    int i = 0;

    if ( ferror( file ) )
        return fclose( file ), 0;

    for ( ; !feof( file ) && i < stat_count; ++i )
    {
        ssize_t len = getline( &buf, &allocated, file ) - 1;  // -1 cause of \n
        buf[ len ] = '\0';                                    // remove newline

        char* line = buf;

        if ( strncmp( line, "cpu", 3 ) != 0 )
            break;

        line += 3;

        // skip cpu digits
        while ( len > 0 && isdigit( line[ 0 ] ) )
            ++line;

        proc_stat_read_str( stat + i, line );
    }

    free( buf );

    fclose( file );

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
         // count as processor working
         + stat->fields[ GUEST ]
         + stat->fields[ GUEST_NICE ];
}
