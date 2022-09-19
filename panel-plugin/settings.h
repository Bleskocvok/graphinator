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

    // TODO: add gtk ui thingies

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


#endif // __SETTINGS_H__
