#pragma once

#ifndef __PANEL_ENTRY_H__
#define __PANEL_ENTRY_H__


#include "drawing.h"
#include "collector.h"

#include <stddef.h>     // size_t

#include <gtk/gtk.h>    // GtkWidget


typedef struct
{
    collector_t collector;

    int interval;

    const char* label_fmt;

    graph_t graph;

    draw_func_t draw_func;

} section_t;


typedef struct
{
    section_t* section;

    GtkWidget* draw_area;
    GtkWidget* label;

    unsigned timer;

    char* label_markup_fmt;
    char* label_buffer;
    size_t label_bsize;

} panel_entry_t;


typedef struct
{
    panel_entry_t* ptr;
    size_t count;
    size_t alloc;

} entries_t;


void entry_refresh( panel_entry_t* entry );
void entry_set_interval( panel_entry_t* entry, int new_ms );

void entries_init( entries_t* entries, size_t reserved );
void entries_free( entries_t* entries );
void entries_add( entries_t* entries, GtkBox* box, section_t* sec );


#endif // __PANEL_ENTRY_H__
