
// getline
#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

// gtk, xfce
#include <gtk/gtk.h>
#include <libxfce4panel/libxfce4panel.h>

// custom
#include "graphinator.h"
#include "cpu_info.h"
#include "data.h"

// c
#include <stdlib.h>         // NULL
#include <stdio.h>          // snprintf


//
// The plugin entry point
//

static void plugin_construct( XfcePanelPlugin* plugin );

XFCE_PANEL_PLUGIN_REGISTER( plugin_construct );


void section_init( section_t* section, section_settings_t* settings )
{
    *section = (section_t){ 0 };
    data_init( &section->measurements, settings->history_size );
    section->collect = settings->collect;
    section->max_value = settings->max_value;
}


static void panel_free( XfcePanelPlugin* plugin, gpointer ptr )
{
    panel_t* pan = ptr;

    // destroy the encapsulating widget and all children
    gtk_widget_destroy( pan->ebox );

    g_slice_free( panel_t, pan );
}


static gboolean collector( gpointer ptr )
{
    section_t* section = ptr;

    double val = section->collect( section->ptr );

    section->last_value = val;
    
    data_push( &section->measurements, val );

    gtk_widget_queue_draw_area( section->widget, 0, 0, 24, 24 );

    return G_SOURCE_CONTINUE;
}


static double collect_cpu_data( void* ptr )
{
    proc_stat_t* prev_stat = ptr;

    proc_stat_t stat = { 0 };

    proc_stat_read( &stat, 1 );

    unsigned long long total =  ( proc_stat_total( &stat )
                                - proc_stat_total( prev_stat ) );

    double frac = total == 0 ? 0
                             : ( (double) ( proc_stat_work( &stat )
                                          - proc_stat_work( prev_stat ) )
                               / (double) total );

    // char buf[ 256 ] = { 0 };
    // snprintf( buf, sizeof( buf ), "cpu %d %%", (int)( frac * 100 ) );

    // gtk_label_set_text( GTK_LABEL( pan->label ), buf );

    *prev_stat = stat;

    return frac * 100;
}


static void draw( GtkWidget* widget, cairo_t* cr, gpointer ptr )
{
    section_t* section = ptr;

    // TODO
    guint w = 30,
          h = 24;
    gtk_render_background( gtk_widget_get_style_context( widget ),
                           cr, 0, 0, w, h );

    int rows = h / 2;
    int cols = data_capacity( &section->measurements );
    int count = data_count( &section->measurements );

    for ( int y = 0; y < rows; ++y )
    {
        for ( int x = 0; x < cols; ++x )
        {
            if ( x >= cols - count
                && ( rows - y ) * section->max_value / (double)rows
                   < data_at( &section->measurements, x - cols + count) )
                cairo_set_source_rgb( cr, 1, 0.5, 0.5 );
            else
                cairo_set_source_rgb( cr, 0.4, 0.4, 0.4 );

            cairo_rectangle( cr,
                             3 * x,
                             2 * y,
                             2,
                             1 );
            cairo_fill( cr );
        }
    }
}


static section_t section = { 0 };
static proc_stat_t prev_stat = { 0 };


static void plugin_construct( XfcePanelPlugin* plugin )
{
    panel_t* pan = g_slice_new( panel_t );

    pan->plugin = plugin;

    pan->ebox = gtk_event_box_new();
    gtk_widget_show( pan->ebox );
    gtk_container_add( GTK_CONTAINER( plugin ), pan->ebox );

    pan->wrap = gtk_box_new( xfce_panel_plugin_get_orientation( plugin ), 2 );
    gtk_widget_show( pan->wrap );
    gtk_container_add( GTK_CONTAINER( pan->ebox ), pan->wrap );

    pan->label = gtk_label_new( "" );
    gtk_widget_show( pan->label );
    gtk_widget_set_size_request( GTK_WIDGET( pan->label ), 100, 24 );
    gtk_box_pack_start( GTK_BOX( pan->wrap ), pan->label, FALSE, FALSE, 0 );

    g_signal_connect( G_OBJECT( plugin ), "free-data",
                      G_CALLBACK( panel_free ), pan );

    section_settings_t settings =
    {
        .interval = 300,
        .history_size = 10,
        .max_value = 100,
        .collect = collect_cpu_data,
    };

    section_init( &section, &settings );
    section.ptr = &prev_stat;

    proc_stat_read( &prev_stat, 1 );

    g_timeout_add( settings.interval, G_SOURCE_FUNC( collector ), &section );

    pan->draw_area = gtk_drawing_area_new();
    gtk_widget_set_size_request( pan->draw_area, 30, 24 );
    gtk_widget_show( pan->draw_area );
    gtk_widget_set_valign( pan->draw_area, GTK_ALIGN_CENTER );
    gtk_box_pack_start( GTK_BOX( pan->wrap ), pan->draw_area, FALSE, FALSE, 0 );
    g_signal_connect( G_OBJECT( pan->draw_area ), "draw",
                      G_CALLBACK( draw ), &section );

    section.widget = pan->draw_area;
}
