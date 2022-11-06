
#include "panel_entry.h"

#include <string.h>     // strlen
#include <stdlib.h>     // calloc, realloc, free


#define MAX_F_CHARS 256


static gboolean collector( gpointer ptr );
static void redraw( GtkWidget* widget, cairo_t* cr, section_t* sec );


void entry_refresh( panel_entry_t* entry )
{
    gtk_widget_set_size_request( entry->draw_area, entry->section->graph.w,
                                                   entry->section->graph.h );
    data_resize( &entry->section->graph.data,
                 graph_cols( &entry->section->graph ) );
}


void entry_refresh_label( panel_entry_t* ent )
{
    if ( ent->section->label_enabled )
    {
        // free the buffers if they have been allocated already
        g_free( ent->label_markup_fmt );
        free( ent->label_buffer );

        ent->label_markup_fmt = g_markup_printf_escaped(
                                        "<tt>"
                                        "<span size=\"small\">"
                                        "%s\n%%%d.%df%s"
                                        "</span>"
                                        "</tt>",
                                        ent->section->label_str,
                                        ent->section->label_digits,
                                        ent->section->label_decimals,
                                        ent->section->label_unit );

        ent->label_bsize = strlen( ent->label_markup_fmt );
        ent->label_buffer = calloc( MAX_F_CHARS + ent->label_bsize, 1 );
    }
}


void entry_set_interval( panel_entry_t* entry, int new_ms )
{
    // remove old timer
    g_source_remove( entry->timer );

    // add new timer
    entry->section->interval = new_ms;
    entry->timer = g_timeout_add( new_ms, G_SOURCE_FUNC( collector ), entry );
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
        collector_free( &entries->ptr[ i ].section->collector );

        g_free( entries->ptr[ i ].label_markup_fmt );
        free( entries->ptr[ i ].label_buffer );
    }

    free( entries->ptr );
    *entries = (entries_t){ 0 };
}


void entries_add( entries_t* entries, GtkBox* box, section_t* sec )
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
    if ( sec->label_enabled )
    {
        entry->label = gtk_label_new( "" );
        gtk_widget_show( entry->label );
        gtk_label_set_justify( GTK_LABEL( entry->label ), GTK_JUSTIFY_RIGHT );
        gtk_widget_set_margin_end( entry->label, 5 );
        gtk_widget_set_margin_start( entry->label, 10 );
        gtk_box_pack_start( box, entry->label, FALSE, FALSE, 0 );

        entry_refresh_label( entry );
    }

    // setup interval
    entry->timer = g_timeout_add( sec->interval, G_SOURCE_FUNC( collector ),
                                                 entry );

    // setup draw area
    entry->draw_area = gtk_drawing_area_new();
    gtk_widget_set_size_request( entry->draw_area, sec->graph.w, sec->graph.h );
    gtk_widget_show( entry->draw_area );
    gtk_widget_set_valign( entry->draw_area, GTK_ALIGN_CENTER );
    gtk_box_pack_start( box, entry->draw_area, FALSE, FALSE, 0 );
    g_signal_connect( G_OBJECT( entry->draw_area ), "draw",
                                                    G_CALLBACK( redraw ), sec );
}


static void entry_update_label( panel_entry_t* ent, float val )
{
    snprintf( ent->label_buffer, ent->label_bsize, ent->label_markup_fmt, val );
    gtk_label_set_markup( GTK_LABEL( ent->label ), ent->label_buffer );
}


static gboolean collector( gpointer ptr )
{
    panel_entry_t* ent = ptr;

    double val = collector_collect( &ent->section->collector );

    data_push( &ent->section->graph.data, val );

    gtk_widget_queue_draw_area( ent->draw_area, 0, 0, ent->section->graph.w,
                                                      ent->section->graph.h );

    if ( ent->section->label_enabled )
    {
        entry_update_label( ent, val );
    }

    return G_SOURCE_CONTINUE;
}


// trampoline
static void redraw( GtkWidget* widget, cairo_t* cr, section_t* sec )
{
    sec->draw_func( widget, cr, &sec->graph );
}
