
// c
#include <stdlib.h>         // calloc, free

#include "data.h"


void data_init( data_t* data, size_t capacity )
{
    *data = (data_t){ 0 };
    data->vals = calloc( capacity, sizeof( *data->vals ) );
    data->capacity = capacity;
}


void data_free( data_t* data )
{
    free( data->vals );
    *data = (data_t){ 0 };
}


size_t data_count( const data_t* data )
{
    return data->count;
}


size_t data_capacity( const data_t* data )
{
    return data->capacity;
}


double data_at( const data_t* data, size_t i )
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


void data_resize( data_t* data, size_t new_cap )
{
    if ( new_cap == data->capacity )
        return;

    data_t new_data = { 0 };
    data_init( &new_data, new_cap );

    for ( size_t i = 0; i < data->count; i++ )
        data_push( &new_data, data_at( data, i ) );

    data_free( data );
    *data = new_data;
}
