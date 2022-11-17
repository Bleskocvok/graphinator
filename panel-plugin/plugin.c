
#include "plugin.h"

#include "defaults.h"
#include "graphinator.h"    // plugin_construct_in_container, panel_t
#include "settings.h"       // settings_construct
#include "utils.h"

#include <stdio.h>          // snprintf
#include <stdlib.h>         // NULL
#include <string.h>         // strncat

// gtk, xfce
#include <gtk/gtk.h>
#include <libxfce4panel/libxfce4panel.h>  // XfcePanelPlugin
                                          // XFCE_PANEL_PLUGIN_REGISTER
#include <libxfce4ui/libxfce4ui.h>        // xfce_titled_dialog_new_with_buttons


//
// The plugin entry point
//
static void plugin_construct( XfcePanelPlugin* plugin );
XFCE_PANEL_PLUGIN_REGISTER( plugin_construct );


//
// Other forwards declarations
//


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
    panel_t* panel;             // (non-owning ptr)

} settings_dialog_t;


static void plugin_settings_close( GtkWidget* dialog,
                                   gint response,
                                   settings_dialog_t* data )
{
    (void) response;

    g_object_set_data( G_OBJECT( data->plugin ), "dialog", NULL );
    xfce_panel_plugin_unblock_menu( data->plugin );
    gtk_widget_destroy( dialog );

    plugin_save( data->plugin, data->panel );

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
    data->panel = pan;

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

    GdkPixbuf* ico = xfce_panel_pixbuf_from_source( "graphinator-plugin",
                                                    NULL, 128 );

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

    plugin_load( plugin, pan );

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

    g_signal_connect( G_OBJECT( plugin ), "save",
                      G_CALLBACK( plugin_save ), pan );
}


static void entries_save( XfceRc* rc, const entries_t* entries )
{
    xfce_rc_write_int_entry( rc, "entries_count", (int) entries->count );

    char buf[ 256 ] = { 0 };

    for ( unsigned i = 0; i < entries->count; i++ )
    {
        section_t* s = entries->ptr[ i ].section;

        snprintf( buf, sizeof( buf ), "entry%u_monitor", i );
        xfce_rc_write_int_entry( rc, buf, find_collector( MONITORS_COL,
                                                          MONITORS_COUNT,
                                                          &s->collector ) );

        snprintf( buf, sizeof( buf ), "entry%u_interval", i );
        xfce_rc_write_int_entry( rc, buf, s->interval );

        snprintf( buf, sizeof( buf ), "entry%u_graph", i );
        xfce_rc_write_int_entry( rc, buf, find_draw_func( GRAPHS_FUNC,
                                                          GRAPHS_COUNT,
                                                          s->draw_func ) );

        snprintf( buf, sizeof( buf ), "entry%u_show_label", i );
        xfce_rc_write_bool_entry( rc, buf, s->label_enabled );

        snprintf( buf, sizeof( buf ), "entry%u_label", i );
        xfce_rc_write_entry( rc, buf, s->label_str );

        snprintf( buf, sizeof( buf ), "entry%u_1_color_r", i );
        xfce_rc_write_int_entry( rc, buf, s->graph.rgb_on[ 0 ] * 255 );

        snprintf( buf, sizeof( buf ), "entry%u_1_color_g", i );
        xfce_rc_write_int_entry( rc, buf, s->graph.rgb_on[ 1 ] * 255 );

        snprintf( buf, sizeof( buf ), "entry%u_1_color_b", i );
        xfce_rc_write_int_entry( rc, buf, s->graph.rgb_on[ 2 ] * 255 );

        snprintf( buf, sizeof( buf ), "entry%u_2_color_r", i );
        xfce_rc_write_int_entry( rc, buf, s->graph.rgb_off[ 0 ] * 255 );

        snprintf( buf, sizeof( buf ), "entry%u_2_color_g", i );
        xfce_rc_write_int_entry( rc, buf, s->graph.rgb_off[ 1 ] * 255 );

        snprintf( buf, sizeof( buf ), "entry%u_2_color_b", i );
        xfce_rc_write_int_entry( rc, buf, s->graph.rgb_off[ 2 ] * 255 );

        snprintf( buf, sizeof( buf ), "entry%u_w", i );
        xfce_rc_write_int_entry( rc, buf, s->graph.w );

        snprintf( buf, sizeof( buf ), "entry%u_h", i );
        xfce_rc_write_int_entry( rc, buf, s->graph.h );

        snprintf( buf, sizeof( buf ), "entry%u_blk_w", i );
        xfce_rc_write_int_entry( rc, buf, s->graph.blk_w );

        snprintf( buf, sizeof( buf ), "entry%u_blk_h", i );
        xfce_rc_write_int_entry( rc, buf, s->graph.blk_h );

        snprintf( buf, sizeof( buf ), "entry%u_pad_x", i );
        xfce_rc_write_int_entry( rc, buf, s->graph.pad_x );

        snprintf( buf, sizeof( buf ), "entry%u_pad_y", i );
        xfce_rc_write_int_entry( rc, buf, s->graph.pad_y );
    }
}


static void entries_load( XfceRc* rc, entries_t* entries )
{
    unsigned count = xfce_rc_read_int_entry( rc, "entries_count",
                                             default_sections_count );

    char buf[ 256 ] = { 0 };

    int entries_count = entries->count;

    // for ( unsigned i = 0; i < count - entries_count; i++ )
    // {
    //     section_t s = default_sections[ 0 ];
    //     section_init( &s );
    //     entries_add( entries, &s );
    // }

    for ( unsigned i = 0; i < M_MIN( entries->count, count ); i++ )
    {
        section_t new_section = { 0 };
        section_t* s = &new_section;

        section_t def = i < default_sections_count ? default_sections[ i ]
                                                   : new_section;

        *s = def;

        snprintf( buf, sizeof( buf ), "entry%u_monitor", i );
        int collector = xfce_rc_read_int_entry( rc, buf, -1 );
        s->collector = collector == -1
                     ? def.collector
                     : *MONITORS_COL[ M_MIN( collector, MONITORS_COUNT - 1 ) ];

        snprintf( buf, sizeof( buf ), "entry%u_interval", i );
        s->interval = xfce_rc_read_int_entry( rc, buf, s->interval );

        snprintf( buf, sizeof( buf ), "entry%u_graph", i );
        int draw = xfce_rc_read_int_entry( rc, buf, -1 );
        s->draw_func = draw == -1
                     ? def.draw_func
                     : GRAPHS_FUNC[ M_MIN( draw, GRAPHS_COUNT - 1 ) ];

        snprintf( buf, sizeof( buf ), "entry%u_show_label", i );
        s->label_enabled = xfce_rc_read_bool_entry( rc, buf,
                                                    s->label_enabled );

        snprintf( buf, sizeof( buf ), "entry%u_label", i );
        const char* str = xfce_rc_read_entry( rc, buf, def.label_str );
        s->label_str[ 0 ] = '\0';
        strncat( s->label_str, str, LABEL_MAX_LEN - 1 );

        double r = 0, g = 0, b = 0;

        snprintf( buf, sizeof( buf ), "entry%u_1_color_r", i );
        r = xfce_rc_read_int_entry( rc, buf, s->graph.rgb_on[ 0 ] * 255 );

        snprintf( buf, sizeof( buf ), "entry%u_1_color_g", i );
        g = xfce_rc_read_int_entry( rc, buf, s->graph.rgb_on[ 1 ] * 255 );

        snprintf( buf, sizeof( buf ), "entry%u_1_color_b", i );
        b = xfce_rc_read_int_entry( rc, buf, s->graph.rgb_on[ 2 ] * 255 );

        s->graph.rgb_on[ 0 ] = r / 255.0;
        s->graph.rgb_on[ 1 ] = g / 255.0;
        s->graph.rgb_on[ 2 ] = b / 255.0;

        snprintf( buf, sizeof( buf ), "entry%u_2_color_r", i );
        r = xfce_rc_read_int_entry( rc, buf, s->graph.rgb_off[ 0 ] * 255 );

        snprintf( buf, sizeof( buf ), "entry%u_2_color_g", i );
        g = xfce_rc_read_int_entry( rc, buf, s->graph.rgb_off[ 1 ] * 255 );

        snprintf( buf, sizeof( buf ), "entry%u_2_color_b", i );
        b = xfce_rc_read_int_entry( rc, buf, s->graph.rgb_off[ 2 ] * 255 );

        s->graph.rgb_off[ 0 ] = r / 255.0;
        s->graph.rgb_off[ 1 ] = g / 255.0;
        s->graph.rgb_off[ 2 ] = b / 255.0;

        snprintf( buf, sizeof( buf ), "entry%u_w", i );
        s->graph.w = xfce_rc_read_int_entry( rc, buf, s->graph.w );

        snprintf( buf, sizeof( buf ), "entry%u_h", i );
        s->graph.h = xfce_rc_read_int_entry( rc, buf, s->graph.h );

        snprintf( buf, sizeof( buf ), "entry%u_blk_w", i );
        s->graph.blk_w = xfce_rc_read_int_entry( rc, buf, s->graph.blk_w );

        snprintf( buf, sizeof( buf ), "entry%u_blk_h", i );
        s->graph.blk_h = xfce_rc_read_int_entry( rc, buf, s->graph.blk_h );

        snprintf( buf, sizeof( buf ), "entry%u_pad_x", i );
        s->graph.pad_x = xfce_rc_read_int_entry( rc, buf, s->graph.pad_x );

        snprintf( buf, sizeof( buf ), "entry%u_pad_y", i );
        s->graph.pad_y = xfce_rc_read_int_entry( rc, buf, s->graph.pad_y );

        // TODO: solve if sections/entries need to be removed instead
        // if ( i < entries_count )
        // {
            section_replace( entries->ptr[ i ].section, s );
            section_init( entries->ptr[ i ].section );
            entry_refresh( &entries->ptr[ i ] );
        // }
        // else
        // {
        //     section_init( s );
        //     entries_add( entries, s );
        // }
    }
}


void plugin_save( XfcePanelPlugin* plugin, const panel_t* data )
{
    char* filename = xfce_panel_plugin_save_location( plugin, TRUE );
    if ( !filename )
        return;

    XfceRc* rc = xfce_rc_simple_open( filename, FALSE );
    g_free( filename );

    if ( rc )
        entries_save( rc, &data->entries );

    xfce_rc_close( rc );
}



void plugin_load( XfcePanelPlugin* plugin, panel_t* data )
{
    char* filename = xfce_panel_plugin_save_location( plugin, TRUE );
    if ( !filename )
        return;

    XfceRc* rc = xfce_rc_simple_open( filename, TRUE );
    g_free( filename );

    if ( rc )
        entries_load( rc, &data->entries );

    xfce_rc_close( rc );
}
