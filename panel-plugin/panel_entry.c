
#include "panel_entry.h"


static gboolean collector( gpointer ptr );
static void redraw( GtkWidget* widget, cairo_t* cr, section_t* sec );


void entry_refresh( panel_entry_t* entry )
{
    gtk_widget_set_size_request( entry->draw_area, entry->section->graph.w,
                                                   entry->section->graph.h );
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
    if ( sec->label_fmt )
    {
        entry->label = gtk_label_new( "" );
        gtk_widget_show( entry->label );
        gtk_box_pack_start( box, entry->label, FALSE, FALSE, 0 );
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


static gboolean collector( gpointer ptr )
{
    panel_entry_t* ent = ptr;

    double val = collector_collect( &ent->section->collector );

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


// trampoline
static void redraw( GtkWidget* widget, cairo_t* cr, section_t* sec )
{
    sec->draw_func( widget, cr, &sec->graph );
}
