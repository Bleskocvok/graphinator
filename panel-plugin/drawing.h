#pragma once

#ifndef __DRAWING_H__
#define __DRAWING_H__

// custom
#include "data.h"

// gtk
#include <gtk/gtk.h>


#define  M_MIN( x, y )  ( ( ( x ) > ( y ) ) ? ( y ) : ( x ) )
#define  M_MAX( x, y )  ( ( ( x ) > ( y ) ) ? ( x ) : ( y ) )


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


typedef void   ( *draw_func_t )    ( GtkWidget* widget,
                                     cairo_t* cr,
                                     void* sec );

void draw_led( GtkWidget* widget, cairo_t* cr, void* ptr );
void draw_lin( GtkWidget* widget, cairo_t* cr, void* ptr );


#endif // __DRAWING_H__
