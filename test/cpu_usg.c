
#include "../panel-plugin/cpu_info.h"

// usleep
#define _BSD_SOURCE
#define _DEFAULT_SOURCE

#include <unistd.h>         // usleep

#include <stdio.h>          // snprintf, printf


int main()
{
    proc_stat_t stat = { 0 };
    proc_stat_t prev_stat = { 0 };

    proc_stat_read( &prev_stat, 1 );

    for (int i = 0; i < 10; i++)
    {
        proc_stat_read( &stat, 1 );

        unsigned long long total = 
          ( proc_stat_total( &stat ) - proc_stat_total( &prev_stat ) );

        double frac = total == 0
            ? 0
            : ( (double) ( proc_stat_work(  &stat )
                         - proc_stat_work(  &prev_stat ) )
              / (double) total );

        char buf[ 256 ] = { 0 };
        snprintf( buf, sizeof( buf ), "cpu %d", (int)( frac * 100 ) );

        printf( "read   %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu \n",
                stat.fields[ USER ],
                stat.fields[ NICE ],
                stat.fields[ SYSTEM ],
                stat.fields[ IDLE ],
                stat.fields[ IOWAIT ],
                stat.fields[ IRQ ],
                stat.fields[ SOFTIRQ ],
                stat.fields[ STEAL ],
                stat.fields[ GUEST ],
                stat.fields[ GUEST_NICE ] );
        printf( "%s\n", buf );

        prev_stat = stat;

        usleep( 750 * 1e3 );
    }
    return 0;
}
