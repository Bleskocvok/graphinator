
#include "defaults.h"


section_t default_sections[] =
{
    {
        .collector = (collector_t){ .collect = collect_cpu_data,
                                    .init = init_cpu_data,
                                    .free = free_cpu_data,       },

        .graph = (graph_t){ .blk_w =  2, .blk_h =  1,
                            .pad_x =  1, .pad_y =  1,
                            .h     = 28, .w     = 33,
                            .rgb_on  = MK_RGB( 255, 128, 128 ),
                            .rgb_off = MK_RGB( 102, 102, 102 ),
                            .max_value = 100,                    },

        .draw_func = draw_lin,
        .interval = 750,
        .label_fmt = " cpu\n%3.0f%% ",
    },

    {
        .collector = (collector_t){ .collect = collect_mem_data,
                                    .init = init_mem_data,
                                    .free = free_mem_data,       },

        .graph = (graph_t){ .blk_w =  2, .blk_h =  1,
                            .pad_x =  1, .pad_y =  1,
                            .h     = 28, .w     = 33,
                            .rgb_on  = MK_RGB( 221, 187,  51 ),
                            .rgb_off = MK_RGB( 102, 102, 102 ),
                            .max_value = 100,                    },

        .draw_func = draw_lin,
        .interval = 750,
        .label_fmt = " mem\n%3.0f%% ",
    },
};


size_t default_sections_count = M_COUNT( default_sections );
