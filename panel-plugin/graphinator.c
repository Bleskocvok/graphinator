
// getline
#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

// gtk, xfce
#include <gtk/gtk.h>
#include <libxfce4panel/libxfce4panel.h>

// custom
#include "graphinator.h"
#include "cpu_info.h"

// c
#include <stdlib.h>         // NULL
#include <stdio.h>          // snprintf


//
// The plugin entry point
//

static void plugin_construct( XfcePanelPlugin* plugin );

XFCE_PANEL_PLUGIN_REGISTER( plugin_construct );


static void panel_free( XfcePanelPlugin* plugin, gpointer ptr )
{
    panel_t* pan = ptr;

    // destroy the encapsulating widget and all children
    gtk_widget_destroy( pan->ebox );

    g_slice_free( panel_t, pan );
}


static gboolean collect_data( gpointer ptr )
{
    panel_t* pan = ptr;

    proc_stat_t stat = { 0 };

    proc_stat_read( &stat, 1 );

    unsigned long long total =  ( proc_stat_total( &stat )
                                - proc_stat_total( &pan->prev_stat ) );

    double frac = total == 0 ? 0
                             : ( (double) ( proc_stat_work( &stat )
                                          - proc_stat_work( &pan->prev_stat ) )
                               / (double) total );

    char buf[ 256 ] = { 0 };
    snprintf( buf, sizeof( buf ), "cpu %d %%", (int)( frac * 100 ) );

    gtk_label_set_text( GTK_LABEL( pan->label ), buf );

    pan->prev_stat = stat;

    return G_SOURCE_CONTINUE;
}


static void draw( GtkWidget* widget, cairo_t* cr, gpointer ptr )
{
    // TODO
    guint w = 24,
          h = 24;
    gtk_render_background( gtk_widget_get_style_context( widget ),
                           cr, 0, 0, w, h );

    cairo_arc( cr,
               w / 2.0, h / 2.0,
               w / 2.0,
               0, 2 * G_PI );

    cairo_set_source_rgb( cr, 128, 255, 128 );

    cairo_fill( cr );
}


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

    pan->draw_area = gtk_drawing_area_new();
    gtk_widget_set_size_request( pan->draw_area, 24, 24 );
    gtk_widget_show( pan->draw_area );
    gtk_widget_set_valign( GTK_WIDGET( pan->draw_area ), GTK_ALIGN_CENTER );
    gtk_box_pack_start( GTK_BOX( pan->wrap ), pan->draw_area, FALSE, FALSE, 0 );
    g_signal_connect( G_OBJECT( pan->draw_area ), "draw",
                      G_CALLBACK( draw ), NULL );

    proc_stat_read( &pan->prev_stat, 1 );
    g_timeout_add( 750, G_SOURCE_FUNC( collect_data ), pan );

    g_signal_connect( G_OBJECT( plugin ), "free-data",
                      G_CALLBACK( panel_free ), pan );
}
