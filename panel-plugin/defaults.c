
#include "defaults.h"

#include "utils.h"
#include "collector.h"


section_t default_sections[] =
{
    {
        .collector = CPU_COLLECTOR_INIT,

        .graph = (graph_t){ .blk_w =  2, .blk_h =  1,
                            .pad_x =  1, .pad_y =  1,
                            .h     = 28, .w     = 55,
                            .rgb_on  = MK_RGB( 255, 128, 128 ),
                            .rgb_off = MK_RGB( 102, 102, 102 ),
                            .max_value = 1,                    },

        .draw_func = draw_lin,
        .interval = 750,

        .label_enabled  = 1,
        .label_str      = "cpu:",
        .label_decimals = 0,
        .label_digits   = 3,
    },

    {
        .collector = MEM_COLLECTOR_INIT,

        .graph = (graph_t){ .blk_w =  2, .blk_h =  1,
                            .pad_x =  1, .pad_y =  1,
                            .h     = 28, .w     = 55,
                            .rgb_on  = MK_RGB( 221, 187,  51 ),
                            .rgb_off = MK_RGB( 102, 102, 102 ),
                            .max_value = 1,                    },

        .draw_func = draw_lin,
        .interval = 750,

        .label_enabled  = 1,
        .label_str      = "mem:",
        .label_decimals = 0,
        .label_digits   = 3,
    },
};


size_t default_sections_count = M_COUNT( default_sections );
// size_t default_sections_count = 1;
