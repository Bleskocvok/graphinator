
// gtk
#include <gtk/gtk.h>

// custom
#include "graphinator.h"
#include "cpu_info.h"
#include "mem_info.h"
#include "data.h"

// c
#include <stdlib.h>         // NULL
#include <stdio.h>          // snprintf


void add_sections( panel_t* pan, section_t* sections, size_t count )
{
    entries_init( &pan->entries, count );

    // data_init( &section.graph.data, graph_cols( &section.graph ) );
    // section.collector.ptr = section.collector.init();

    // data_init( &sec_mem.graph.data, graph_cols( &sec_mem.graph ) );
    // sec_mem.collector.ptr = sec_mem.collector.init();

    // entries_add( &pan->entries, pan, &section );
    // entries_add( &pan->entries, pan, &sec_mem );

    for ( size_t i = 0; i < count; i++ )
    {
        section_t* sec = sections + i;

        data_init( &sec->graph.data, graph_cols( &sec->graph ) );
        sec->collector.ptr = sec->collector.init();

        entries_add( &pan->entries, GTK_BOX( pan->wrap ), sec );
    }
}


panel_t* plugin_construct_in_container( GtkContainer* container,
                                        GtkOrientation orient )
{
    panel_t* pan = g_slice_new( panel_t );
    *pan = (panel_t){ 0 };

    pan->ebox = gtk_event_box_new();
    gtk_widget_show( pan->ebox );
    gtk_container_add( GTK_CONTAINER( container ), pan->ebox );

    pan->wrap = gtk_box_new( orient, 2 );
    gtk_widget_show( pan->wrap );
    gtk_container_add( GTK_CONTAINER( pan->ebox ), pan->wrap );

    return pan;
}
