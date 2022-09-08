
#include "settings.h"

#include <stdlib.h>     // NULL


void add_page( GtkNotebook* notebook )
{
    GtkWidget* page = gtk_grid_new();
    gtk_widget_show( page );
    gtk_grid_set_column_homogeneous( GTK_GRID( page ), TRUE );
    gtk_grid_set_row_spacing( GTK_GRID( page ), 10 );

    int idx = gtk_notebook_append_page( notebook, page, NULL );
    gtk_notebook_set_tab_reorderable( notebook, page, TRUE );
    (void) idx;

    const int label_height = 24;
    int row = 0;

    {
        GtkWidget* label = gtk_label_new( "Primary color:" );
        gtk_widget_show( label );
        gtk_label_set_xalign( GTK_LABEL( label ), 0.9 );
        gtk_widget_set_size_request( label, -1, label_height );
        gtk_grid_attach( GTK_GRID( page ), label, 0, row, 1, 1 );
    }

    {
        GtkWidget* color_chooser = gtk_color_button_new();
        gtk_widget_show( color_chooser );
        gtk_grid_attach( GTK_GRID( page ), color_chooser, 1, row, 1, 1 );
    }

    ++row;

    {
        GtkWidget* label = gtk_label_new( "Secondary color:" );
        gtk_widget_show( label );
        gtk_label_set_xalign( GTK_LABEL( label ), 0.9 );
        gtk_widget_set_size_request( label, -1, label_height );
        gtk_grid_attach( GTK_GRID( page ), label, 0, row, 1, 1 );
    }

    {
        GtkWidget* color_chooser = gtk_color_button_new();
        gtk_widget_show( color_chooser );
        gtk_grid_attach( GTK_GRID( page ), color_chooser, 1, row, 1, 1 );
    }

    ++row;

    {
        GtkWidget* label = gtk_label_new( "Width:" );
        gtk_widget_show( label );
        gtk_label_set_xalign( GTK_LABEL( label ), 0.9 );
        gtk_widget_set_size_request( label, -1, label_height );
        gtk_grid_attach( GTK_GRID( page ), label, 0, row, 1, 1 );
    }

    {
        GtkWidget* number = gtk_spin_button_new_with_range( 0, 9999, 1 );
        gtk_widget_show( number );
        gtk_grid_attach( GTK_GRID( page ), number, 1, row, 1, 1 );
    }

    ++row;

    {
        GtkWidget* label = gtk_label_new( "Height:" );
        gtk_widget_show( label );
        gtk_label_set_xalign( GTK_LABEL( label ), 0.9 );
        gtk_widget_set_size_request( label, -1, label_height );
        gtk_grid_attach( GTK_GRID( page ), label, 0, row, 1, 1 );
    }

    {
        GtkWidget* number = gtk_spin_button_new_with_range( 0, 9999, 1 );
        gtk_widget_show( number );
        gtk_grid_attach( GTK_GRID( page ), number, 1, row, 1, 1 );
    }

    ++row;


    {
        GtkWidget* label = gtk_label_new( "Graph mode:" );
        gtk_widget_show( label );
        gtk_label_set_xalign( GTK_LABEL( label ), 0.9 );
        gtk_widget_set_size_request( label, -1, label_height );
        gtk_grid_attach( GTK_GRID( page ), label, 0, row, 1, 1 );
    }

    {
        GtkWidget* drop_down = gtk_combo_box_text_new_with_entry();
        gtk_widget_show( drop_down );

        gtk_combo_box_text_append_text( GTK_COMBO_BOX_TEXT( drop_down ),
                                        "Normal" );
        gtk_combo_box_text_append_text( GTK_COMBO_BOX_TEXT( drop_down ),
                                        "Bars" );
        gtk_combo_box_text_append_text( GTK_COMBO_BOX_TEXT( drop_down ),
                                        "LED" );

        gtk_combo_box_set_active( GTK_COMBO_BOX( drop_down ), 0 );
        gtk_grid_attach( GTK_GRID( page ), drop_down, 1, row, 1, 1 );
    }

    ++row;

    {
        GtkWidget* label = gtk_label_new( "Monitor:" );
        gtk_widget_show( label );
        gtk_label_set_xalign( GTK_LABEL( label ), 0.9 );
        gtk_widget_set_size_request( label, -1, label_height );
        gtk_grid_attach( GTK_GRID( page ), label, 0, row, 1, 1 );
    }

    {
        GtkWidget* drop_down = gtk_combo_box_text_new_with_entry();
        gtk_widget_show( drop_down );

        gtk_combo_box_text_append_text( GTK_COMBO_BOX_TEXT( drop_down ),
                                        "CPU" );
        gtk_combo_box_text_append_text( GTK_COMBO_BOX_TEXT( drop_down ),
                                        "Memory" );
        gtk_combo_box_text_append_text( GTK_COMBO_BOX_TEXT( drop_down ),
                                        "Custom" );

        gtk_combo_box_set_active( GTK_COMBO_BOX( drop_down ), 0 );
        gtk_grid_attach( GTK_GRID( page ), drop_down, 1, row, 1, 1 );
    }

    ++row;

    {
        GtkWidget* check = gtk_check_button_new_with_label( "Show label" );
        gtk_widget_show( check );
        gtk_grid_attach( GTK_GRID( page ), check, 0, row, 1, 1 );
    }

    {
        GtkWidget* entry = gtk_entry_new();
        gtk_widget_show( entry );
        gtk_grid_attach( GTK_GRID( page ), entry, 1, row, 1, 1 );
    }
}


void settings_construct( settings_t* settings, GtkWidget* container )
{
    GtkWidget* new_but = gtk_button_new_with_label( "new section" );
    gtk_widget_show( new_but );
    gtk_widget_set_size_request( new_but, 128, 20 );
    gtk_container_add( GTK_CONTAINER( container ), new_but );

    GtkNotebook* notebook = GTK_NOTEBOOK( gtk_notebook_new() );
    settings->notebook = GTK_WIDGET( notebook );

    gtk_widget_show( settings->notebook );
    gtk_container_add( GTK_CONTAINER( container ), settings->notebook );

    add_page( notebook );
    add_page( notebook );

    // {
    //     GtkWidget* page = gtk_grid_new();
    //     gtk_widget_show( page );

    //     GtkWidget* but = gtk_button_new_with_label( "hello there" );
    //     gtk_widget_show( but );
    //     gtk_grid_attach( GTK_GRID( page ), but, 0, 0, 1, 1 );

    //     GtkWidget* color_chooser = gtk_color_button_new();
    //     gtk_widget_show( color_chooser );
    //     gtk_grid_attach( GTK_GRID( page ), color_chooser, 5, 1, 1, 1 );

    //     int idx = gtk_notebook_append_page( notebook, page, NULL );
    //     (void) idx;
    //     gtk_notebook_set_tab_reorderable( notebook, page, TRUE );
    // }

    // {
    //     GtkWidget* lab = gtk_label_new( "label" );
    //     gtk_widget_show( lab );

    //     gtk_notebook_append_page( notebook, lab, NULL );
    //     gtk_notebook_set_tab_reorderable( notebook, lab, TRUE );
    // }
}
