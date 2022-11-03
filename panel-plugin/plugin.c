
#include "plugin.h"

#include "defaults.h"
#include "graphinator.h"    // plugin_construct_in_container, panel_t
#include "settings.h"       // settings_construct
#include "utils.h"

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


typedef struct
{
    settings_t* settings;       //     (owning ptr)
    XfcePanelPlugin* plugin;    // (non-owning ptr)

} settings_dialog_t;


static void plugin_settings_close( GtkWidget* dialog,
                                   gint response,
                                   settings_dialog_t* data )
{
    g_object_set_data( G_OBJECT( data->plugin ), "dialog", NULL );
    xfce_panel_plugin_unblock_menu( data->plugin );
    gtk_widget_destroy( dialog );

    // free settings_dialog_t
    settings_free( data->settings );
    free( data->settings );
    free( data );
}


static void plugin_settings( XfcePanelPlugin* plugin, panel_t* pan )
{
    GtkWindow* parent;
    GtkWidget* dia;
    settings_dialog_t* data;

    // init settings_dialog_t
    data = calloc( 1, sizeof( settings_dialog_t ) );
    data->settings = calloc( 1, sizeof( settings_t ) );
    data->plugin = plugin;

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
                                       data );

    gtk_window_set_default_size( GTK_WINDOW( dia ), 300, 300 );
    gtk_widget_show( dia );

    GtkWidget* box = gtk_box_new( GTK_ORIENTATION_VERTICAL, 6 );
    gtk_widget_show( box );
    GtkWidget* content = gtk_dialog_get_content_area( GTK_DIALOG( dia ) );
    gtk_box_pack_start( GTK_BOX( content ), box, FALSE, FALSE, 0 );
    gtk_container_set_border_width( GTK_CONTAINER( content ), 10 );

    settings_construct( data->settings, box, &pan->entries );
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
                           "copyright",    M_U8"Copyright © 2022 " AUTHOR "\n",
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

    add_sections( pan, default_sections, default_sections_count );

    g_signal_connect( G_OBJECT( plugin ), "free-data",
                      G_CALLBACK( panel_free ), pan );

    g_signal_connect( G_OBJECT( plugin ), "orientation-changed",
                      G_CALLBACK( orientation_changed ), pan );

    xfce_panel_plugin_menu_show_about( plugin );
    g_signal_connect( G_OBJECT( plugin ), "about", G_CALLBACK( plugin_about ),
                                          NULL );

    xfce_panel_plugin_menu_show_configure( plugin );
    g_signal_connect( G_OBJECT( plugin ), "configure-plugin",
                                          G_CALLBACK( plugin_settings ), pan );
}
