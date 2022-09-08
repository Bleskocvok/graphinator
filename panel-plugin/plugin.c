
#include "plugin.h"

#include "graphinator.h"    // plugin_construct_in_container, panel_t
#include "settings.h"       // settings_construct

// gtk, xfce
#include <gtk/gtk.h>
#include <libxfce4panel/libxfce4panel.h>
#include <libxfce4ui/libxfce4ui.h>        // xfce_titled_dialog_new_with_buttons


//
// The plugin entry point
//
static void plugin_construct( XfcePanelPlugin* plugin );
XFCE_PANEL_PLUGIN_REGISTER( plugin_construct );


static void panel_free( XfcePanelPlugin* plugin, gpointer ptr )
{
    (void) plugin;

    panel_t* pan = ptr;

    // destroy the encapsulating widget and all children
    gtk_widget_destroy( pan->ebox );

    for ( size_t i = 0; i < pan->entries.count; ++i )
    {
        data_free( &pan->entries.ptr[ i ].section->graph.data );
        collector_t* coll = &pan->entries.ptr[ i ].section->collector;
        coll->free( coll->ptr );
    }

    entries_free( &pan->entries );

    g_slice_free( panel_t, pan );
}


static void orientation_changed( XfcePanelPlugin* plugin,
                                 GtkOrientation orientation,
                                 panel_t* pan )
{
    (void) plugin;

    gtk_orientable_set_orientation( GTK_ORIENTABLE( pan->wrap ), orientation );
    // for ( size_t i = 0; i < pan->entries.count; ++i )
    // {
    //     gtk_orientable_set_orientation( GTK_ORIENTABLE(
    //                                         pan->entries.ptr[ i ].label ),
    //                                     orientation );
    //     gtk_orientable_set_orientation( GTK_ORIENTABLE(
    //                                         pan->entries.ptr[ i ].draw_area ),
    //                                     orientation );
    // }
}


static void plugin_settings_close( GtkWidget* dialog,
                                   gint response,
                                   XfcePanelPlugin* plugin )
{
    g_object_set_data( G_OBJECT( plugin ), "dialog", NULL );
    xfce_panel_plugin_unblock_menu( plugin );
    gtk_widget_destroy( dialog );
}


static void plugin_settings( XfcePanelPlugin* plugin, panel_t* pan )
{
    GtkWindow* parent;
    GtkWidget* dia;

    xfce_panel_plugin_block_menu( plugin );

    parent = GTK_WINDOW( gtk_widget_get_toplevel( GTK_WIDGET( plugin ) ) );

    dia = xfce_titled_dialog_new_with_buttons( "Graphinator settings",
                                               parent,
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               "gtk-close", GTK_RESPONSE_OK,
                                               NULL );

    gtk_window_set_position( GTK_WINDOW( dia ), GTK_WIN_POS_CENTER );
    gtk_window_set_icon_name( GTK_WINDOW( dia ), "graphinator-plugin" );

    g_signal_connect( G_OBJECT( dia ), "response",
                                       G_CALLBACK( plugin_settings_close ),
                                       plugin );

    gtk_window_set_default_size( GTK_WINDOW( dia ), 300, 300 );
    gtk_widget_show( dia );

    GtkWidget* box = gtk_box_new( GTK_ORIENTATION_VERTICAL, 6 );
    gtk_widget_show( box );
    GtkWidget* content = gtk_dialog_get_content_area( GTK_DIALOG( dia ) );
    gtk_box_pack_start( GTK_BOX( content ), box, FALSE, FALSE, 0 );

    settings_t settings = { 0 };
    settings_construct( &settings, box );
}


#define AUTHOR  "František Bráblík"
#define WEBSITE "https://github.com/Bleskocvok/graphinator"

static void plugin_about( XfcePanelPlugin* plugin )
{
    (void) plugin;

    GdkPixbuf* ico = xfce_panel_pixbuf_from_source( "graphinator-plugin", NULL, 128 );

    gtk_show_about_dialog( NULL,
                           "logo",         ico,
                           "license",      "IDK yet",
                           "version",      "0.0.1",
                           "program-name", "graphinator",
                           "comments",     "Graphinator is a plugin",
                           "website",      WEBSITE,
                           "copyright",    u8"Copyright © 2022 " AUTHOR "\n",
                           "authors",      (const char*[]){ AUTHOR, NULL },
                           NULL );

    if ( ico )
        g_object_unref( G_OBJECT( ico ) );
}


static void plugin_construct( XfcePanelPlugin* plugin )
{
    GtkOrientation orient = xfce_panel_plugin_get_orientation( plugin );
    panel_t* pan = plugin_construct_in_container( GTK_CONTAINER( plugin ),
                                                  orient );

    g_signal_connect( G_OBJECT( plugin ), "free-data",
                      G_CALLBACK( panel_free ), pan );

    g_signal_connect( G_OBJECT( plugin ), "orientation-changed",
                      G_CALLBACK( orientation_changed ), pan );

    xfce_panel_plugin_menu_show_about( plugin );
    g_signal_connect( G_OBJECT( plugin ), "about", G_CALLBACK( plugin_about ),
                                          NULL );

    xfce_panel_plugin_menu_show_configure( plugin );
    g_signal_connect( G_OBJECT( plugin ), "configure-plugin",
                                          G_CALLBACK( plugin_settings ), NULL );

    add_sections( pan );
}
