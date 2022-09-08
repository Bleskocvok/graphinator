#pragma once

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

// gtk, xfce
#include <gtk/gtk.h>


typedef struct
{
    GtkWidget* notebook;

} settings_t;


void settings_construct( settings_t* settings, GtkWidget* container );


#endif // __SETTINGS_H__
