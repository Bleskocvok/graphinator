#pragma once

#ifndef __DRAWING_H__
#define __DRAWING_H__

// custom
#include "data.h"

// gtk
#include <gtk/gtk.h>


#define  MK_RGB( r, g, b )  { r / 255.0, g / 255.0, b / 255.0 }


typedef struct
{
    int blk_w,
        blk_h;

    int pad_x,
        pad_y;

    int w,
        h;

    double rgb_off[ 3 ];
    double rgb_on[ 3 ];

    data_t data;
    double max_value;

} graph_t;


int graph_cols( const graph_t* graph );
int graph_rows( const graph_t* graph );
double graph_max_value( const graph_t* graph );


typedef void   ( *draw_func_t )    ( GtkWidget* widget,
                                     cairo_t* cr,
                                     void* ptr );

void draw_led( GtkWidget* widget, cairo_t* cr, void* ptr );
void draw_lin( GtkWidget* widget, cairo_t* cr, void* ptr );
void draw_bar( GtkWidget* widget, cairo_t* cr, void* ptr );


#endif // __DRAWING_H__
