#pragma once

#ifndef __PANEL_ENTRY_H__
#define __PANEL_ENTRY_H__


#include "drawing.h"
#include "collector.h"

#include <stddef.h>     // size_t

#include <gtk/gtk.h>    // GtkWidget


#define LABEL_MAX_LEN 256


typedef struct
{
    collector_t collector;

    int interval;

    int label_enabled;
    char label_str[ LABEL_MAX_LEN ];
    int label_decimals;
    int label_digits;

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
    GtkBox* box;

    panel_entry_t* ptr;
    size_t count;
    size_t alloc;

} entries_t;


void section_init( section_t* section );
void section_free( section_t* section );

void entry_refresh( panel_entry_t* entry );
void entry_set_interval( panel_entry_t* entry, int new_ms );
void entry_refresh_label( panel_entry_t* entry );

void entries_init( entries_t* entries, size_t reserved, GtkBox* box );
void entries_free( entries_t* entries );
void entries_add( entries_t* entries, section_t* sec );
void entries_reset( entries_t* entries, size_t count );

void entries_refresh_all( entries_t* entries );

#endif // __PANEL_ENTRY_H__
