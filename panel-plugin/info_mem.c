
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


int proc_mem_read( proc_mem_t* mem_info )
{
    static const char* mem_info_filename = "/proc/meminfo";

    FILE* file = fopen( mem_info_filename, "r" );

    char* buf = NULL;
    size_t allocated = 0;

    int i = 0;

    *mem_info = (proc_mem_t){ 0 };

    if ( ferror( file ) )
        return fclose( file ), 0;

    for ( ; !feof( file ) && i < 3; ++i )
    {
        ssize_t len = getline( &buf, &allocated, file ) - 1;  // -1 cause of \n
        buf[ len ] = '\0';                                    // remove newline

        char* line = buf;
        unsigned long num = 0;

        if ( IS_PREFIX( line, "MemTotal:" ) )
        {
            line += sizeof( "MemTotal:" );
            parse_data( line, &num );
            mem_info->total = num;
        }
        else if ( IS_PREFIX( line, "MemFree:" ) )
        {
            line += sizeof( "MemFree:" );
            parse_data( line, &num );
            mem_info->free = num;
        }
        else if ( IS_PREFIX( line, "MemAvailable:" ) )
        {
            line += sizeof( "MemAvailable:" );
            parse_data( line, &num );
            mem_info->avail = num;
        }
        else
        {
            // we expect the first three lines to contain the three
            // if one of them doesn't, return with error
            return -1;
        }
    }

    free( buf );

    fclose( file );

    return 0;
}

