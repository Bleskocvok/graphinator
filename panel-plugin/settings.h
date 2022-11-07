#pragma once

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

// custom
#include "graphinator.h"
#include "panel_entry.h"

// gtk, xfce
#include <gtk/gtk.h>


typedef struct
{
    GtkWidget* notebook;

    entries_t* entries;         // (non-owning ptr)

    struct page_t* pages;
    size_t pages_count;

} settings_t;


typedef struct page_t
{
    panel_entry_t* entry;       // (non-owning ptr)
    settings_t* settings;       // (non-owning ptr)

    GtkWidget* combo_mon;
    GtkWidget* combo_graph;
    GtkWidget* spin_inter;
    GtkWidget* check_label;
    GtkWidget* entry_label;
    GtkWidget* color_prim;
    GtkWidget* color_secon;
    GtkWidget* spin_w;
    GtkWidget* spin_h;
    GtkWidget* spin_blk_w;
    GtkWidget* spin_blk_h;
    GtkWidget* spin_pad_x;
    GtkWidget* spin_pad_y;

} page_t;


void settings_construct( settings_t* settings,
                         GtkWidget* container,
                         entries_t* entries );

void settings_free( settings_t* settings );


#endif // __SETTINGS_H__
