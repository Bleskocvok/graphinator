
#include "settings.h"

#include <stdlib.h>     // NULL, calloc


void add_entry( GtkButton* self, void* ptr ) {}
void remove_entry( GtkButton* self, void* ptr ) {}

void set_monitor( GtkComboBox* self, void* ptr ) {}
void set_interval( GtkButton* self, void* ptr ) {}
void set_graph_mode( GtkComboBox* self, void* ptr ) {}
void toggle_label( GtkButton* self, void* ptr ) {}
void set_label( GtkButton* self, void* ptr ) {}
void set_primary_color( GtkButton* self, void* ptr ) {}
void set_secondary_color( GtkButton* self, void* ptr ) {}
void set_graph_w( GtkButton* self, void* ptr ) {}
void set_graph_h( GtkButton* self, void* ptr ) {}
void set_graph_blk_w( GtkButton* self, void* ptr ) {}
void set_graph_blk_h( GtkButton* self, void* ptr ) {}
void set_graph_pad_w( GtkButton* self, void* ptr ) {}
void set_graph_pad_h( GtkButton* self, void* ptr ) {}


void add_page( GtkNotebook* notebook, page_t* info )
{
    GtkWidget* page = gtk_grid_new();
    gtk_widget_show( page );
    gtk_grid_set_column_homogeneous( GTK_GRID( page ), TRUE );
    gtk_grid_set_row_spacing( GTK_GRID( page ), 10 );
    gtk_grid_set_column_spacing( GTK_GRID( page ), 6 );
    gtk_container_set_border_width( GTK_CONTAINER( page ), 6 );

    int idx = gtk_notebook_append_page( notebook, page, NULL );
    gtk_notebook_set_tab_reorderable( notebook, page, TRUE );
    (void) idx;

    const int label_height = 24;
    const int wide = 4;
    const int narrow = 2;
    const double xalign = 1;
    int row = 0;

    {
        GtkWidget* label = gtk_label_new( "Monitor:" );
        gtk_widget_show( label );
        gtk_label_set_xalign( GTK_LABEL( label ), xalign );
        gtk_widget_set_size_request( label, -1, label_height );
        gtk_grid_attach( GTK_GRID( page ), label, 0, row, wide, 1 );
    }
    {
        GtkWidget* drop_down = gtk_combo_box_text_new();
        gtk_widget_show( drop_down );

        GtkComboBoxText* combo = GTK_COMBO_BOX_TEXT( drop_down );
        gtk_combo_box_text_append_text( combo, "CPU" );
        gtk_combo_box_text_append_text( combo, "Memory" );
        gtk_combo_box_text_append_text( combo, "Custom" );

        gtk_combo_box_set_active( GTK_COMBO_BOX( drop_down ), 0 );
        gtk_grid_attach( GTK_GRID( page ), drop_down, wide, row, wide, 1 );

        g_signal_connect( combo, "changed", G_CALLBACK( set_monitor ), NULL );
    }
    ++row;

    {
        GtkWidget* label = gtk_label_new( "Update interval (ms):" );
        gtk_widget_show( label );
        gtk_label_set_xalign( GTK_LABEL( label ), xalign );
        gtk_widget_set_size_request( label, -1, label_height );
        gtk_grid_attach( GTK_GRID( page ), label, 0, row, wide, 1 );
    }
    {
        GtkWidget* number = gtk_spin_button_new_with_range( 0, 9999999, 1 );
        gtk_widget_show( number );
        gtk_grid_attach( GTK_GRID( page ), number, wide, row, wide, 1 );
    }
    ++row;

    {
        GtkWidget* label = gtk_label_new( "Graph mode:" );
        gtk_widget_show( label );
        gtk_label_set_xalign( GTK_LABEL( label ), xalign );
        gtk_widget_set_size_request( label, -1, label_height );
        gtk_grid_attach( GTK_GRID( page ), label, 0, row, wide, 1 );
    }
    {
        GtkWidget* drop_down = gtk_combo_box_text_new();
        gtk_widget_show( drop_down );

        GtkComboBoxText* combo = GTK_COMBO_BOX_TEXT( drop_down );
        gtk_combo_box_text_append_text( combo, "Normal" );
        gtk_combo_box_text_append_text( combo, "Bars" );
        gtk_combo_box_text_append_text( combo, "LED" );

        gtk_combo_box_set_active( GTK_COMBO_BOX( drop_down ), 0 );
        gtk_grid_attach( GTK_GRID( page ), drop_down, wide, row, wide, 1 );

        g_signal_connect( combo, "changed", G_CALLBACK( set_graph_mode ), NULL );
    }
    ++row;

    {
        GtkWidget* check = gtk_check_button_new_with_label( "Show label" );
        gtk_widget_show( check );
        gtk_grid_attach( GTK_GRID( page ), check, 0, row, wide, 1 );
        gtk_widget_set_halign( check, GTK_ALIGN_END );
    }
    {
        GtkWidget* entry = gtk_entry_new();
        gtk_widget_show( entry );
        gtk_grid_attach( GTK_GRID( page ), entry, wide, row, wide, 1 );
        gtk_entry_set_placeholder_text( GTK_ENTRY( entry ), "Label" );
        gtk_widget_set_sensitive( entry, FALSE );
    }
    ++row;

    {
        GtkWidget* label = gtk_label_new( "Primary color:" );
        gtk_widget_show( label );
        gtk_label_set_xalign( GTK_LABEL( label ), xalign );
        gtk_widget_set_size_request( label, -1, label_height );
        gtk_grid_attach( GTK_GRID( page ), label, 0, row, wide, 1 );
    }
    {
        GtkWidget* color_chooser = gtk_color_button_new();
        gtk_widget_show( color_chooser );
        gtk_grid_attach( GTK_GRID( page ), color_chooser, wide, row, wide, 1 );
    }
    ++row;

    {
        GtkWidget* label = gtk_label_new( "Secondary color:" );
        gtk_widget_show( label );
        gtk_label_set_xalign( GTK_LABEL( label ), xalign );
        gtk_widget_set_size_request( label, -1, label_height );
        gtk_grid_attach( GTK_GRID( page ), label, 0, row, wide, 1 );
    }
    {
        GtkWidget* color_chooser = gtk_color_button_new();
        gtk_widget_show( color_chooser );
        gtk_grid_attach( GTK_GRID( page ), color_chooser, wide, row, wide, 1 );
    }
    ++row;

    {
        GtkWidget* label = gtk_label_new( u8"Width ⨯ Height:" );
        gtk_widget_show( label );
        gtk_label_set_xalign( GTK_LABEL( label ), xalign );
        gtk_widget_set_size_request( label, -1, label_height );
        gtk_grid_attach( GTK_GRID( page ), label, 0, row, wide, 1 );
    }
    {
        GtkWidget* number = gtk_spin_button_new_with_range( 0, 9999, 1 );
        gtk_widget_show( number );
        gtk_grid_attach( GTK_GRID( page ), number, wide, row, narrow, 1 );
    }
    {
        GtkWidget* number = gtk_spin_button_new_with_range( 0, 9999, 1 );
        gtk_widget_show( number );
        gtk_grid_attach( GTK_GRID( page ), number, wide + narrow, row, narrow, 1 );
    }
    ++row;

    {
        GtkWidget* label = gtk_label_new( u8"Block Width ⨯ Block Height:" );
        gtk_widget_show( label );
        gtk_label_set_xalign( GTK_LABEL( label ), xalign );
        gtk_widget_set_size_request( label, -1, label_height );
        gtk_grid_attach( GTK_GRID( page ), label, 0, row, wide, 1 );
    }
    {
        GtkWidget* number = gtk_spin_button_new_with_range( 0, 9999, 1 );
        gtk_widget_show( number );
        gtk_grid_attach( GTK_GRID( page ), number, wide, row, narrow, 1 );
    }
    {
        GtkWidget* number = gtk_spin_button_new_with_range( 0, 9999, 1 );
        gtk_widget_show( number );
        gtk_grid_attach( GTK_GRID( page ), number, wide + narrow, row, narrow, 1 );
    }
    ++row;

    {
        GtkWidget* label = gtk_label_new( u8"Spacing X ⨯ Spacing Y:" );
        gtk_widget_show( label );
        gtk_label_set_xalign( GTK_LABEL( label ), xalign );
        gtk_widget_set_size_request( label, -1, label_height );
        gtk_grid_attach( GTK_GRID( page ), label, 0, row, wide, 1 );
    }
    {
        GtkWidget* number = gtk_spin_button_new_with_range( 0, 9999, 1 );
        gtk_widget_show( number );
        gtk_grid_attach( GTK_GRID( page ), number, wide, row, narrow, 1 );
    }
    {
        GtkWidget* number = gtk_spin_button_new_with_range( 0, 9999, 1 );
        gtk_widget_show( number );
        gtk_grid_attach( GTK_GRID( page ), number, wide + narrow, row, narrow, 1 );
    }
    ++row;

    {
        GtkWidget* but = gtk_button_new_with_label( "Remove section" );
        gtk_widget_show( but );
        gtk_grid_attach( GTK_GRID( page ), but, 0, row, narrow, 1 );
    }
    ++row;
}


void settings_free( settings_t* settings )
{
    free( settings->pages );
}


void settings_construct( settings_t* settings,
                         GtkWidget* container,
                         entries_t* entries )
{
    GtkWidget* new_but = gtk_button_new_with_label( "Add section" );
    gtk_widget_show( new_but );
    gtk_container_add( GTK_CONTAINER( container ), new_but );

    g_signal_connect( new_but, "clicked", G_CALLBACK( add_entry ), NULL );

    GtkNotebook* notebook = GTK_NOTEBOOK( gtk_notebook_new() );
    settings->notebook = GTK_WIDGET( notebook );

    gtk_widget_show( settings->notebook );
    gtk_container_add( GTK_CONTAINER( container ), settings->notebook );

    settings->entries = entries;
    settings->pages_count = entries->count;
    settings->pages = calloc( entries->count, sizeof( page_t ) );

    for ( size_t i = 0; i < entries->count; i++ )
    {
        settings->pages[ i ].entry = &settings->entries->ptr[ i ];
        add_page( notebook, &settings->pages[ i ] );
    }

    // if ( entries->count > 0 )
        gtk_notebook_set_current_page( notebook, 0 );
}
