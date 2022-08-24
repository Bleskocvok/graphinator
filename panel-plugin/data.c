
// c
#include <stdlib.h>         // calloc, free

#include "data.h"


void data_init( data_t* data, size_t capacity )
{
    *data = (data_t){ 0 };
    data->vals = calloc( capacity, sizeof( *data->vals ) );
    data->capacity = capacity;
}


size_t data_count( data_t* data )
{
    return data->count;
}


size_t data_capacity( data_t* data )
{
    return data->capacity;
}


double data_at( data_t* data, size_t i )
{
    return data->vals[ (i + data->end) % data->capacity ];
}


void data_push( data_t* data, double val )
{
    if (data->count < data->capacity)
    {
        data->vals[ data->count ] = val;
        data->count++;
    }
    else
    {
        data->vals[ data->end ] = val;
        data->end = (data->end + 1) % data->capacity;
    }
}


void data_free( data_t* data )
{
    free( data->vals );
    *data = (data_t){ 0 };
}

