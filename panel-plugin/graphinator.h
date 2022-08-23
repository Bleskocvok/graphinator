#pragma once

#ifndef __GRAPHINATOR_H__
#define __GRAPHINATOR_H__


G_BEGIN_DECLS


#include "cpu_info.h"


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


typedef struct
{
    double* vals;
    size_t capacity;
    size_t count;
    size_t end;

} data_t;


G_END_DECLS


#endif // __GRAPHINATOR_H__
