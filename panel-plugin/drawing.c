#include "drawing.h"

// gtk
#include <gtk/gtk.h>


int graph_cols( const graph_t* graph )
{
    return graph->w / ( graph->blk_w + graph->pad_x );
}


double graph_max_value( const graph_t* graph )
{
    if ( graph->max_value > 0 )
        return graph->max_value;

    double max = -1;
    for ( size_t i = 0; i < data_count( &graph->data ); i++ )
        if ( max < data_at( &graph->data, i ) )
            max = data_at( &graph->data, i );
    return max;
}


void draw_led( GtkWidget* widget, cairo_t* cr, void* ptr )
{
    graph_t* sec = ptr;
    const graph_t g = *sec;

    int rows = g.h / ( g.blk_h + g.pad_y );
    int cols = graph_cols( &g );
    int count = data_count( &sec->data );

    double max_value = graph_max_value( &g );
    double blk = max_value / (double) rows;

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


void draw_lin( GtkWidget* widget, cairo_t* cr, void* ptr )
{
    graph_t* sec = ptr;
    const graph_t g = *sec;

    int cols = graph_cols( &g );
    int count = data_count( &sec->data );

    double max_value = graph_max_value( &g );
    double blk = g.h / (double) max_value;

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

