
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


// static section_t section =
// {
//     .collector = (collector_t){ .collect = collect_cpu_data,
//                                 .init = init_cpu_data,
//                                 .free = free_cpu_data,       },

//     .graph = (graph_t){ .blk_w =  2, .blk_h =  1,
//                         .pad_x =  1, .pad_y =  1,
//                         .h     = 28, .w     = 33,
//                         .rgb_on  = MK_RGB( 255, 128, 128 ),
//                         .rgb_off = MK_RGB( 102, 102, 102 ),
//                         .max_value = 100,                    },

//     .draw_func = draw_lin,
//     .interval = 750,
//     .label_fmt = " cpu\n%3.0f%% ",
// };


// static section_t sec_mem =
// {
//     .collector = (collector_t){ .collect = collect_mem_data,
//                                 .init = init_mem_data,
//                                 .free = free_mem_data,       },

//     .graph = (graph_t){ .blk_w =  2, .blk_h =  1,
//                         .pad_x =  1, .pad_y =  1,
//                         .h     = 28, .w     = 33,
//                         .rgb_on  = MK_RGB( 221, 187,  51 ),
//                         .rgb_off = MK_RGB( 102, 102, 102 ),
//                         .max_value = 100,                    },

//     .draw_func = draw_lin,
//     .interval = 750,
//     .label_fmt = " mem\n%3.0f%% ",
// };


void entry_refresh( panel_entry_t* entry )
{
    gtk_widget_set_size_request( entry->draw_area, entry->section->graph.w,
                                                   entry->section->graph.h );
}


static gboolean collector( gpointer ptr )
{
    panel_entry_t* ent = ptr;

    double val = ent->section->collector.collect( ent->section->collector.ptr );
    
    data_push( &ent->section->graph.data, val );

    gtk_widget_queue_draw_area( ent->draw_area, 0, 0, ent->section->graph.w,
                                                      ent->section->graph.h );

    // TODO: optimize probably
    // TODO: also make more general
    if ( ent->section->label_fmt )
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


void* init_mem_data( void )
{
    return NULL;
}


void free_mem_data( void* ptr )
{
    (void) ptr;
}


double collect_mem_data( void* ptr )
{
    (void) ptr;

    proc_mem_t info;
    proc_mem_read( &info );
    if ( info.total == 0 )
        return -1;
    return ( info.total - info.avail ) / (double) info.total * 100;
}


void* init_cpu_data( void )
{
    proc_stat_t* ptr = calloc( 1, sizeof( proc_stat_t ) );
    if ( !ptr )
        return NULL;

    proc_stat_read( ptr, 1 );
    return ptr;
}


void free_cpu_data( void* ptr )
{
    free( ptr );
}


double collect_cpu_data( void* ptr )
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


void entries_init( entries_t* entries, size_t reserved )
{
    *entries = (entries_t){ 0 };
    entries->ptr = calloc( reserved, sizeof( panel_entry_t ) );
    entries->alloc = reserved;
}


void entries_free( entries_t* entries )
{
    for ( size_t i = 0; i < entries->count; ++i )
    {
        data_free( &entries->ptr[ i ].section->graph.data );
        collector_t* coll = &entries->ptr[ i ].section->collector;
        coll->free( coll->ptr );
    }

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
    if ( sec->label_fmt )
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
                      G_CALLBACK( entry->section->draw_func ), &sec->graph );
}


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

        entries_add( &pan->entries, pan, sec );
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
