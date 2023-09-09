
#include "info_mem.h"

// getline
#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

// c
#include <stdlib.h>         // NULL, free, strtoull
#include <stdio.h>          // FILE, fopen, fclose, feof, ferror
#include <string.h>         // strncmp
#include <ctype.h>          // isdigit, isspace

// posix
#include <stdio.h>          // getline


#define IS_PREFIX( str, pre ) \
    ( strncmp( line, pre, sizeof( pre ) - 1 ) == 0 )


int proc_mem_init( proc_mem_t* info )
{
    *info = (proc_mem_t){ 0 };
    return 0;
}


void proc_mem_free( proc_mem_t* info )
{
    free( info->buf );
}


static int parse_data( const char* str, unsigned long* num )
{
    char* end;

    // skip whitespace
    while ( isspace( str[ 0 ] ) )
        ++str;

    // parse number
    *num = strtoull( str, &end, 10 );
    if ( end == str )
        return -1;

    // TODO: parse unit

    return 0;
}


int proc_mem_read( proc_mem_t* info )
{
    FILE* file = fopen( "/proc/meminfo", "r" );

    if ( ferror( file ) )
        return fclose( file ), -1;

    int i = 0;

    for ( ; !feof( file ) && i < 3; ++i )
    {
        // -1 cause of \n
        ssize_t len = getline( &info->buf, &info->allocated, file ) - 1;
        // remove newline
        info->buf[ len ] = '\0';

        char* line = info->buf;
        unsigned long num = 0;

        if ( IS_PREFIX( line, "MemTotal:" ) )
        {
            line += sizeof( "MemTotal:" );
            parse_data( line, &num );
            info->total = num;
        }
        else if ( IS_PREFIX( line, "MemFree:" ) )
        {
            line += sizeof( "MemFree:" );
            parse_data( line, &num );
            info->free = num;
        }
        else if ( IS_PREFIX( line, "MemAvailable:" ) )
        {
            line += sizeof( "MemAvailable:" );
            parse_data( line, &num );
            info->avail = num;
        }
        else
        {
            // we expect the first three lines to contain the three
            // if one of them doesn't, return with error
            return -1;
        }
    }

    fclose( file );

    return 0;
}

