#pragma once

#include <stdlib.h>         // size_t


typedef struct
{
    double* vals;
    size_t capacity;
    size_t count;
    size_t end;

} data_t;


void data_init( data_t* data, size_t capacity );

void data_free( data_t* data );

size_t data_count( data_t* data );

size_t data_capacity( data_t* data );

double data_at( data_t* data, size_t i );

void data_push( data_t* data, double val );
