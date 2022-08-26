
#include "../panel-plugin/data.h"

#include <stdio.h>          // printf
#include <stdlib.h>         // size_t


int main()
{
    data_t data = { 0 };
    data_init( &data, 10 );

    for ( int n = 0; n < 100; ++n )
    {
        data_push( &data, n );
        for ( size_t i = 0; i < data_count( &data ); ++i )
            printf( "%3d ", (int) data_at( &data, i ) );
        printf( "%3zd / %3zd", data_count( &data ), data_capacity( &data ) );
        printf( "\n" );
    }

    data_free( &data );
    return 0;
}
