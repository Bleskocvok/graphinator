#pragma once

#ifndef __GRAPHINATOR_H__
#define __GRAPHINATOR_H__


#include <gtk/gtk.h>    // G_BEGIN_DECLS, GtkWidget, GtkContainer,
                        // GtkOrientation


G_BEGIN_DECLS


#include "panel_entry.h"


typedef struct
{
    GtkWidget* ebox;
    GtkWidget* wrap;

    entries_t entries;

} panel_t;


panel_t* plugin_construct_in_container( GtkContainer* container,
                                        GtkOrientation orient );

void add_sections( panel_t* pan, section_t* sections, size_t count );


G_END_DECLS


#endif // __GRAPHINATOR_H__
