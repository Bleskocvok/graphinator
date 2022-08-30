
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


#define  M_MIN( x, y )  ( ( ( x ) > ( y ) ) ? ( y ) : ( x ) )
#define  M_MAX( x, y )  ( ( ( x ) > ( y ) ) ? ( x ) : ( y ) )


//
// The plugin entry point
//
static void plugin_construct( XfcePanelPlugin* plugin );
XFCE_PANEL_PLUGIN_REGISTER( plugin_construct );


static void panel_free( XfcePanelPlugin* plugin, gpointer ptr );


static void* init_cpu_data( void );
static void free_cpu_data( void* ptr );
static double collect_cpu_data( void* ptr );


static void* init_mem_data( void );
static void free_mem_data( void* ptr );
static double collect_mem_data( void* ptr );


static section_t section =
{
    .collector = (collector_t){ .collect = collect_cpu_data,
                                .init = init_cpu_data,
                                .free = free_cpu_data,       },

    .graph = (graph_t){ .blk_w =  2, .blk_h =  1,
                        .pad_x =  1, .pad_y =  1,
                        .h     = 28, .w     =  0,
                        .rgb_on  = MK_RGB( 255, 128, 128 ),
                        .rgb_off = MK_RGB( 102, 102, 102 ),  },

    .interval = 750,
    .max_value = 100,
    .use_max_value = true,
    .show_label = true,
    .label_fmt = " cpu\n%3.0f%% ",
};


static section_t sec_mem =
{
    .collector = (collector_t){ .collect = collect_mem_data,
                                .init = init_mem_data,
                                .free = free_mem_data,       },

    .graph = (graph_t){ .blk_w =  2, .blk_h =  1,
                        .pad_x =  1, .pad_y =  1,
                        .h     = 28, .w     =  0,
                        .rgb_on  = MK_RGB( 221, 187,  51 ),
                        .rgb_off = MK_RGB( 102, 102, 102 ),  },

    .interval = 750,
    .max_value = 100,
    .use_max_value = true,
    .show_label = true,
    .label_fmt = " mem\n%3.0f%% ",
};


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


static gboolean collector( gpointer ptr )
{
    panel_entry_t* ent = ptr;

    double val = ent->section->collector.collect( ent->section->collector.ptr );

    ent->section->last_value = val;
    
    data_push( &ent->section->data, val );

    gtk_widget_queue_draw_area( ent->draw_area, 0, 0, ent->section->graph.w,
                                                      ent->section->graph.h );

    // TODO: optimize probably
    // TODO: also make more general
    if ( ent->section->show_label )
    {
        char buff[ 256 ] = { 0 };
        snprintf( buff, sizeof( buff ), ent->section->label_fmt, val );
        // gtk_label_set_text( GTK_LABEL( ent->label ), buff );

        char* markup = g_markup_printf_escaped( "<tt>"
                                                "<span size=\"small\">"
                                                "%s"
                                                "</span>"
                                                "</tt>", buff );
        gtk_label_set_markup( GTK_LABEL( ent->label ), markup );
        g_free( markup );
    }

    return G_SOURCE_CONTINUE;
}


#include <sys/sysinfo.h>
static void* init_mem_data( void )
{
    return NULL;
}


static void free_mem_data( void* ptr )
{
    (void) ptr;
}


static double collect_mem_data( void* ptr )
{
    (void) ptr;

    struct sysinfo info;
    if ( sysinfo( &info ) < 0)
        return 0;

    return 100 - ( info.freeram / (double) info.totalram * 100.0 );
}


static void* init_cpu_data( void )
{
    proc_stat_t* ptr = calloc( 1, sizeof( proc_stat_t ) );
    if ( !ptr )
        return NULL;

    proc_stat_read( ptr, 1 );
    return ptr;
}


static void free_cpu_data( void* ptr )
{
    free( ptr );
}


static double collect_cpu_data( void* ptr )
{
    proc_stat_t* prev_stat = ptr;

    proc_stat_t stat = { 0 };

    proc_stat_read( &stat, 1 );

    unsigned long long total =  ( proc_stat_total( &stat )
                                - proc_stat_total( prev_stat ) );

    double frac = total == 0 ? 0  // <-- avoid division by zero
                             : ( (double) ( proc_stat_work( &stat )
                                          - proc_stat_work( prev_stat ) )
                               / (double) total );

    *prev_stat = stat;

    return frac * 100;
}


static void draw( GtkWidget* widget, cairo_t* cr, gpointer ptr )
{
    section_t* sec = ptr;

    const graph_t g = sec->graph;

    int rows = g.h / ( g.blk_h + g.pad_y );
    int cols = g.w / ( g.blk_w + g.pad_x );

    int cap = data_capacity( &sec->data );
    cols = M_MIN( cap, cols );

    int count = data_count( &sec->data );

    double blk = sec->max_value / (double)rows;

    gtk_render_background( gtk_widget_get_style_context( widget ),
                           cr, 0, 0, g.w, g.h);

    for ( int y = 0; y < rows; ++y )
    {
        for ( int x = 0; x < cols; ++x )
        {
            int i = x + count - cols;
            if ( i >= 0 && ( rows - 1 - y ) * blk <= data_at( &sec->data, i ) )
                cairo_set_source_rgb( cr, g.rgb_on[ 0 ], g.rgb_on[ 1 ],
                                          g.rgb_on[ 2 ] );
            else
                cairo_set_source_rgb( cr, g.rgb_off[ 0 ], g.rgb_off[ 1 ],
                                          g.rgb_off[ 2 ] );

            cairo_rectangle( cr, ( g.blk_w + g.pad_x ) * x,
                                 ( g.blk_h + g.pad_y ) * y,
                                 g.blk_w,
                                 g.blk_h );
            cairo_fill( cr );
        }
    }
}


static void entries_init( entries_t* entries, size_t reserved )
{
    *entries = (entries_t){ 0 };
    entries->ptr = calloc( reserved, sizeof( panel_entry_t ) );
    entries->alloc = reserved;
}


static void entries_free( entries_t* entries )
{
    free( entries->ptr );
    *entries = (entries_t){ 0 };
}


static void entries_add( entries_t* entries, panel_t* pan, section_t* sec )
{
    // add to collection
    if ( entries->count == entries->alloc )
    {
        entries->alloc *= 2;
        if ( !( entries->ptr = realloc( entries->ptr, entries->alloc ) ) )
        {
            // TODO: handle allocation fail
            return;
        }
    }

    entries->ptr[ entries->count ] = (panel_entry_t){ .section = sec };
    panel_entry_t* entry = &entries->ptr[ entries->count ];
    ++entries->count;

    // add label
    if ( sec->show_label )
    {
        entry->label = gtk_label_new( "" );
        gtk_widget_show( entry->label );
        // gtk_widget_set_size_request( GTK_WIDGET( entry->label ), 48, 24 );
        gtk_box_pack_start( GTK_BOX( pan->wrap ), entry->label,
                                                  FALSE, FALSE, 0 );
    }

    // setup interval
    g_timeout_add( sec->interval, G_SOURCE_FUNC( collector ), entry );
    // setup draw area
    entry->draw_area = gtk_drawing_area_new();
    gtk_widget_set_size_request( entry->draw_area, sec->graph.w,
                                                   sec->graph.h );
    gtk_widget_show( entry->draw_area );
    gtk_widget_set_valign( entry->draw_area, GTK_ALIGN_CENTER );
    gtk_box_pack_start( GTK_BOX( pan->wrap ), entry->draw_area,
                                              FALSE, FALSE, 0 );
    g_signal_connect( G_OBJECT( entry->draw_area ), "draw",
                      G_CALLBACK( draw ), sec );
}


static void orientation_changed( XfcePanelPlugin* plugin,
                                 GtkOrientation orientation,
                                 panel_t* pan )
{
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
    panel_t* pan = g_slice_new( panel_t );
    *pan = (panel_t){ 0 };

    pan->plugin = plugin;

    pan->ebox = gtk_event_box_new();
    gtk_widget_show( pan->ebox );
    gtk_container_add( GTK_CONTAINER( plugin ), pan->ebox );

    pan->wrap = gtk_box_new( xfce_panel_plugin_get_orientation( plugin ), 2 );
    gtk_widget_show( pan->wrap );
    gtk_container_add( GTK_CONTAINER( pan->ebox ), pan->wrap );

    g_signal_connect( G_OBJECT( plugin ), "free-data",
                      G_CALLBACK( panel_free ), pan );

    g_signal_connect( G_OBJECT( plugin ), "orientation-changed",
                      G_CALLBACK( orientation_changed ), pan );

    // TODO
    const int history_size = 11;
    data_init( &section.data, history_size );
    section.graph.w = history_size * ( section.graph.blk_w
                                     + section.graph.pad_x );

    section.collector.ptr = section.collector.init();
    if ( !section.collector.ptr )
    {
        // TODO: error
        // TODO: shit, might not be error, therefore refactor necessary
    }

    data_init( &sec_mem.data, history_size );
    sec_mem.graph.w = history_size * ( sec_mem.graph.blk_w
                                     + sec_mem.graph.pad_x );

    sec_mem.collector.ptr = sec_mem.collector.init();

    entries_init( &pan->entries, 10 );
    entries_add( &pan->entries, pan, &section );
    entries_add( &pan->entries, pan, &sec_mem );
}
