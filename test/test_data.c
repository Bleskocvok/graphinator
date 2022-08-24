
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
        printf( "\n" );
    }
    return 0;
}
