
// gtk
#include <gtk/gtk.h>

// custom
#include "graphinator.h"
#include "cpu_info.h"
#include "mem_info.h"
#include "data.h"

// c
#include <stdlib.h>         // NULL, size_t
#include <stdio.h>          // snprintf


const char* MONITORS_STR[ MONITORS_COUNT ] = { "CPU", "Memory" };
const collector_t* MONITORS_COL[ MONITORS_COUNT ] = { &cpu_collector,
                                                      &mem_collector, };

const char* GRAPHS_STR[ GRAPHS_COUNT ] = { "Normal", "LED" };
const draw_func_t GRAPHS_FUNC[ GRAPHS_COUNT ] = { draw_lin, draw_led, };


void add_sections( panel_t* pan, section_t* sections, size_t count )
{
    entries_init( &pan->entries, count, GTK_BOX( pan->wrap ) );

    for ( size_t i = 0; i < count; i++ )
    {
        section_t* sec = sections + i;
        section_init( sec );

        entries_add( &pan->entries, sec );
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


int find_collector( const collector_t** haystack, size_t count,
                    const collector_t* needle )
{
    for ( size_t i = 0; i < count; ++i )
        if ( collector_equals( needle, haystack[ i ] ) )
            return i;
    return -1;
}


int find_draw_func( const draw_func_t* haystack, size_t count,
                    const draw_func_t needle )
{
    for ( size_t i = 0; i < count; ++i )
        if ( needle == haystack[ i ] )
            return i;
    return -1;
}
