#pragma once

#ifndef __GRAPHINATOR_H__
#define __GRAPHINATOR_H__


#include <gtk/gtk.h>    // G_BEGIN_DECLS, GtkWidget, GtkContainer,
                        // GtkOrientation


G_BEGIN_DECLS


#include "cpu_info.h"
#include "data.h"
#include "drawing.h"

#include <stdbool.h>        // bool, true, false


#define M_COUNT( x )  ( sizeof( x ) / sizeof( *x ) )

#define MK_RGB( r, g, b ) { r / 255.0, g / 255.0, b / 255.0 }


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

    const char* label_fmt;

    graph_t graph;

    draw_func_t draw_func;

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
    GtkWidget* ebox;
    GtkWidget* wrap;

    entries_t entries;

} panel_t;


void entries_init( entries_t* entries, size_t reserved );
void entries_free( entries_t* entries );
void entries_add( entries_t* entries, panel_t* pan, section_t* sec );


panel_t* plugin_construct_in_container( GtkContainer* container,
                                        GtkOrientation orient );

void add_sections( panel_t* pan, section_t* sections, size_t count );


void* init_cpu_data( void );
void free_cpu_data( void* ptr );
double collect_cpu_data( void* ptr );


void* init_mem_data( void );
void free_mem_data( void* ptr );
double collect_mem_data( void* ptr );


G_END_DECLS


#endif // __GRAPHINATOR_H__
