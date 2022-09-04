
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


static void* init_cpu_data( void );
static void free_cpu_data( void* ptr );
static double collect_cpu_data( void* ptr );


static void* init_mem_data( void );
static void free_mem_data( void* ptr );
static double collect_mem_data( void* ptr );


static void draw_led( GtkWidget* widget, cairo_t* cr, void* ptr );
static void draw_lin( GtkWidget* widget, cairo_t* cr, void* ptr );


static section_t section =
{
    .collector = (collector_t){ .collect = collect_cpu_data,
                                .init = init_cpu_data,
                                .free = free_cpu_data,       },

    .graph = (graph_t){ .blk_w =  2, .blk_h =  1,
                        .pad_x =  1, .pad_y =  1,
                        .h     = 28, .w     = 33,
                        .rgb_on  = MK_RGB( 255, 128, 128 ),
                        .rgb_off = MK_RGB( 102, 102, 102 ),  },

    .draw_func = draw_lin,
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
                        .h     = 28, .w     = 33,
                        .rgb_on  = MK_RGB( 221, 187,  51 ),
                        .rgb_off = MK_RGB( 102, 102, 102 ),  },

    .draw_func = draw_led,
    .interval = 750,
    .max_value = 100,
    .use_max_value = true,
    .show_label = true,
    .label_fmt = " mem\n%3.0f%% ",
};


int graph_history_size( graph_t* graph )
{
    return graph->w / ( graph->blk_w + graph->pad_x );
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

    proc_mem_t info;
    proc_mem_read( &info );
    if ( info.total == 0 )
        return -1;
    return ( info.total - info.avail ) / (double) info.total * 100;
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


static void draw_led( GtkWidget* widget, cairo_t* cr, void* ptr )
{
    section_t* sec = ptr;

    const graph_t g = sec->graph;

    int rows = g.h / ( g.blk_h + g.pad_y );
    int cols = g.w / ( g.blk_w + g.pad_x );

    int cap = data_capacity( &sec->data );
    cols = M_MIN( cap, cols );

    int count = data_count( &sec->data );

    double blk = sec->max_value / (double) rows;

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


static void draw_lin( GtkWidget* widget, cairo_t* cr, void* ptr )
{
    section_t* sec = ptr;

    const graph_t g = sec->graph;

    int cols = g.w / ( g.blk_w + g.pad_x );

    int cap = data_capacity( &sec->data );
    cols = M_MIN( cap, cols );

    int count = data_count( &sec->data );

    double blk = g.h / (double) sec->max_value;

    gtk_render_background( gtk_widget_get_style_context( widget ),
                           cr, 0, 0, g.w, g.h);

    cairo_set_source_rgb( cr, g.rgb_on[ 0 ], g.rgb_on[ 1 ], g.rgb_on[ 2 ] );

    if ( count <= 0 )
        return;

    double prev = count - cols < 0 ? 0
                                   : blk * data_at( &sec->data, count - cols );
    cairo_move_to( cr, g.w, g.h );
    cairo_line_to( cr,   0, g.h );
    cairo_line_to( cr,   0, g.h - prev );

    for ( int x = 1; x < cols; ++x )
    {
        int i = x + count - cols;
        double y = x < cols - count ? 0
                                    : blk * data_at( &sec->data, i );
        cairo_line_to( cr, ( g.blk_w + g.pad_x ) * x, g.h - y );
        prev = y;
    }

    cairo_line_to( cr, g.w, g.h - prev );
    cairo_close_path( cr );

    cairo_set_line_width( cr, 0.5 );
    cairo_stroke_preserve( cr );

    cairo_fill( cr );
}


void entries_init( entries_t* entries, size_t reserved )
{
    *entries = (entries_t){ 0 };
    entries->ptr = calloc( reserved, sizeof( panel_entry_t ) );
    entries->alloc = reserved;
}


void entries_free( entries_t* entries )
{
    free( entries->ptr );
    *entries = (entries_t){ 0 };
}


void entries_add( entries_t* entries, panel_t* pan, section_t* sec )
{
    // reallocate if necessary
    if ( entries->count == entries->alloc )
    {
        entries->alloc *= 2;
        if ( !( entries->ptr = realloc( entries->ptr, entries->alloc ) ) )
        {
            // TODO: handle allocation fail
            return;
        }
    }

    // add to collection
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
                      G_CALLBACK( entry->section->draw_func ), sec );
}


void add_sections( panel_t* pan )
{
    data_init( &section.data, graph_history_size( &section.graph ) );
    section.collector.ptr = section.collector.init();

    data_init( &sec_mem.data, graph_history_size( &sec_mem.graph ) );
    sec_mem.collector.ptr = sec_mem.collector.init();

    entries_init( &pan->entries, 10 );
    entries_add( &pan->entries, pan, &section );
    entries_add( &pan->entries, pan, &sec_mem );
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
