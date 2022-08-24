#pragma once

#ifndef __GRAPHINATOR_H__
#define __GRAPHINATOR_H__


G_BEGIN_DECLS


#include "cpu_info.h"
#include "data.h"

#include <stdbool.h>        // bool, true, false


typedef struct
{
    XfcePanelPlugin* plugin;

    GtkWidget* ebox;
    GtkWidget* wrap;
    GtkWidget* label;
    GtkWidget* draw_area;

    // cpu info
    proc_stat_t prev_stat;

} panel_t;


typedef double ( *collect_func_t ) ( void* );


typedef struct
{
    int interval;

    int history_size;

    double max_value;

    collect_func_t collect;

} section_settings_t;


typedef struct
{
    data_t measurements;

    double last_value;

    double max_value;

    collect_func_t collect;

    void* ptr;

    GtkWidget* widget;

} section_t;


typedef struct
{
} led_graph_t;


G_END_DECLS


#endif // __GRAPHINATOR_H__
