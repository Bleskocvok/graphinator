#pragma once

#ifndef __GRAPHINATOR_H__
#define __GRAPHINATOR_H__


#include <gtk/gtk.h>    // G_BEGIN_DECLS, GtkWidget, GtkContainer,
                        // GtkOrientation

#include <stddef.h>     // size_t


G_BEGIN_DECLS


#include "panel_entry.h"


#define  MONITORS_COUNT  2
#define  GRAPHS_COUNT    2

extern const char*        MONITORS_STR[ MONITORS_COUNT ];
extern const collector_t* MONITORS_COL[ MONITORS_COUNT ];
extern const char*       GRAPHS_STR[  GRAPHS_COUNT ];
extern const draw_func_t GRAPHS_FUNC[ GRAPHS_COUNT ];


typedef struct
{
    GtkWidget* ebox;
    GtkWidget* wrap;

    entries_t entries;

} panel_t;


panel_t* plugin_construct_in_container( GtkContainer* container,
                                        GtkOrientation orient );

void add_sections( panel_t* pan, section_t* sections, size_t count );


int find_collector( const collector_t** haystack, size_t count,
                    const collector_t* needle );
int find_draw_func( const draw_func_t* haystack, size_t count,
                    const draw_func_t needle );


G_END_DECLS


#endif // __GRAPHINATOR_H__
