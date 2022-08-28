#pragma once

#ifndef __GRAPHINATOR_H__
#define __GRAPHINATOR_H__


G_BEGIN_DECLS


#include "cpu_info.h"
#include "data.h"

#include <stdbool.h>        // bool, true, false


#define M_COUNT( x )  ( sizeof( x ) / sizeof( *x ) )

#define MK_RGB( r, g, b ) { r / 255.0, g / 255.0, b / 255.0 }


typedef struct
{
    int blk_w,
        blk_h;

    int pad_x,
        pad_y;

    int w,
        h;

    double rgb_off[ 3 ];
    double rgb_on[ 3 ];

} graph_t;


typedef double ( *collect_func_t ) ( void* );
typedef void*  ( *init_func_t )    ( void  );
typedef void   ( *free_func_t )    ( void* );

typedef struct
{
    init_func_t init;
    collect_func_t collect;
    free_func_t free;

    void* ptr;

} collector_t;


typedef struct
{
    collector_t collector;

    int interval;

    double max_value;
    const char* label_fmt;

    bool use_max_value : 1;
    bool show_label    : 1;

    graph_t graph;

    data_t data;
    double last_value;

} section_t;


typedef struct
{
    section_t* section;

    GtkWidget* draw_area;
    GtkWidget* label;

} panel_entry_t;


typedef struct
{
    panel_entry_t* ptr;
    size_t count;
    size_t alloc;

} entries_t;


typedef struct
{
    XfcePanelPlugin* plugin;

    GtkWidget* ebox;
    GtkWidget* wrap;

    entries_t entries;

} panel_t;


static void entries_init( entries_t* entries, size_t reserved );
static void entries_free( entries_t* entries );
static void entries_add( entries_t* entries, panel_t* pan, section_t* sec );


G_END_DECLS


#endif // __GRAPHINATOR_H__
