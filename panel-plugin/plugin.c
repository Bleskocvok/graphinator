
#include "plugin.h"

#include "graphinator.h"  // plugin_construct_in_container, panel_t

// gtk, xfce
#include <gtk/gtk.h>
#include <libxfce4panel/libxfce4panel.h>


//
// The plugin entry point
//
static void plugin_construct( XfcePanelPlugin* plugin );
XFCE_PANEL_PLUGIN_REGISTER( plugin_construct );


static void panel_free( XfcePanelPlugin* plugin, gpointer ptr );


static void panel_free( XfcePanelPlugin* plugin, gpointer ptr )
{
    (void) plugin;

    panel_t* pan = ptr;

    // destroy the encapsulating widget and all children
    gtk_widget_destroy( pan->ebox );

    for ( size_t i = 0; i < pan->entries.count; ++i )
    {
        data_free( &pan->entries.ptr[ i ].section->data );
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


static void plugin_construct( XfcePanelPlugin* plugin )
{
    GtkOrientation orient = xfce_panel_plugin_get_orientation( plugin );
    panel_t* pan = plugin_construct_in_container( GTK_CONTAINER( plugin ),
                                                  orient );

    g_signal_connect( G_OBJECT( plugin ), "free-data",
                      G_CALLBACK( panel_free ), pan );

    g_signal_connect( G_OBJECT( plugin ), "orientation-changed",
                      G_CALLBACK( orientation_changed ), pan );

    add_sections( pan );
}