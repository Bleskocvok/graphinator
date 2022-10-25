#pragma once

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

// custom
#include "graphinator.h"

// gtk, xfce
#include <gtk/gtk.h>


typedef struct
{
    panel_entry_t* entry;       // (non-owning ptr)

    GtkWidget* combo_monitor;
    GtkWidget* combo_graph;
    GtkWidget* spin_interval;
    GtkWidget* check_label;
    GtkWidget* entry_label;
    GtkWidget* color_primary;
    GtkWidget* color_secondary;
    GtkWidget* spin_w;
    GtkWidget* spin_h;
    GtkWidget* spin_blk_w;
    GtkWidget* spin_blk_h;
    GtkWidget* spin_pad_x;
    GtkWidget* spin_pad_y;

} page_t;


typedef struct
{
    GtkWidget* notebook;

    entries_t* entries;         // (non-owning ptr)

    page_t* pages;
    size_t pages_count;

} settings_t;


void settings_construct( settings_t* settings,
                         GtkWidget* container,
                         entries_t* entries );

void settings_free( settings_t* settings );


#endif // __SETTINGS_H__
