
#include "settings.h"

#include <stdlib.h>     // NULL, calloc



static const char* MONITORS[] = { "CPU", "Memory", "Custom" };


//
// Forward declarations
//

// Callbacks
void add_entry(             GtkButton*      self,   void*   ptr   );
void remove_entry(          GtkButton*      self,   void*   ptr   );
void set_monitor(           GtkComboBox*    self,   page_t* ptr   );
void set_interval(          GtkSpinButton*  self,   page_t* ptr   );
void set_graph_mode(        GtkComboBox*    self,   page_t* ptr   );
void toggle_label(          GtkCheckButton* self,   page_t* ptr   );
void set_label(             GtkEntry*       self,   page_t* ptr   );
void set_primary_color(     GtkColorButton* self,   page_t* ptr   );
void set_secondary_color(   GtkColorButton* self,   page_t* ptr   );
void set_graph_w(           GtkSpinButton*  self,   page_t* ptr   );
void set_graph_h(           GtkSpinButton*  self,   page_t* ptr   );
void set_graph_blk_w(       GtkSpinButton*  self,   page_t* ptr   );
void set_graph_blk_h(       GtkSpinButton*  self,   page_t* ptr   );
void set_graph_pad_x(       GtkSpinButton*  self,   page_t* ptr   );
void set_graph_pad_y(       GtkSpinButton*  self,   page_t* ptr   );

// Main
void add_page( GtkNotebook* notebook, page_t* ctx );
void settings_free( settings_t* settings );
void settings_construct( settings_t* settings,
                         GtkWidget* container,
                         entries_t* entries );


//
// Utility functions
//
static int read_spin( GtkSpinButton* in )
{
    return gtk_spin_button_get_value_as_int( in );
}


//
// Definitions
//


void add_page( GtkNotebook* notebook, page_t* ctx )
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

        g_signal_connect( combo, "changed", G_CALLBACK( set_monitor ), ctx );
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

        g_signal_connect( number, "value-changed",
                                  G_CALLBACK( set_interval ),
                                  ctx );
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
        // gtk_combo_box_text_append_text( combo, "Bars" );
        gtk_combo_box_text_append_text( combo, "LED" );

        gtk_combo_box_set_active( GTK_COMBO_BOX( drop_down ), 0 );
        gtk_grid_attach( GTK_GRID( page ), drop_down, wide, row, wide, 1 );

        g_signal_connect( combo, "changed", G_CALLBACK( set_graph_mode ), ctx );
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

        // g_signal_connect(  );
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
        GdkRGBA rgba = { .red = 0.0, .green = 0.0, .blue = 0.0, .alpha = 1.0 };
        GtkWidget* color_chooser = gtk_color_button_new_with_rgba( &rgba );
        gtk_widget_show( color_chooser );
        gtk_grid_attach( GTK_GRID( page ), color_chooser, wide, row, wide, 1 );

        g_signal_connect( color_chooser, "color-set",
                                         G_CALLBACK( set_primary_color ),
                                         ctx );
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
        GdkRGBA rgba = { .red = 1.0, .green = 0.0, .blue = 1.0, .alpha = 1.0 };
        GtkWidget* color_chooser = gtk_color_button_new_with_rgba( &rgba );
        gtk_widget_show( color_chooser );
        gtk_grid_attach( GTK_GRID( page ), color_chooser, wide, row, wide, 1 );

        g_signal_connect( color_chooser, "color-set",
                                         G_CALLBACK( set_secondary_color ),
                                         ctx );
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

        g_signal_connect( number, "value-changed",
                                  G_CALLBACK( set_graph_w ),
                                  ctx );
    }
    {
        GtkWidget* number = gtk_spin_button_new_with_range( 0, 9999, 1 );
        gtk_widget_show( number );
        gtk_grid_attach( GTK_GRID( page ), number, wide + narrow, row, narrow, 1 );

        g_signal_connect( number, "value-changed",
                                  G_CALLBACK( set_graph_h ),
                                  ctx );
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

        g_signal_connect( number, "value-changed",
                                  G_CALLBACK( set_graph_blk_w ),
                                  ctx );
    }
    {
        GtkWidget* number = gtk_spin_button_new_with_range( 0, 9999, 1 );
        gtk_widget_show( number );
        gtk_grid_attach( GTK_GRID( page ), number, wide + narrow, row, narrow, 1 );

        g_signal_connect( number, "value-changed",
                                  G_CALLBACK( set_graph_blk_h ),
                                  ctx );
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

        g_signal_connect( number, "value-changed",
                                  G_CALLBACK( set_graph_pad_x ),
                                  ctx );
    }
    {
        GtkWidget* number = gtk_spin_button_new_with_range( 0, 9999, 1 );
        gtk_widget_show( number );
        gtk_grid_attach( GTK_GRID( page ), number, wide + narrow, row, narrow, 1 );

        g_signal_connect( number, "value-changed",
                                  G_CALLBACK( set_graph_pad_y ),
                                  ctx );
    }
    ++row;

    {
        GtkWidget* but = gtk_button_new_with_label( "Remove section" );
        gtk_widget_show( but );
        gtk_grid_attach( GTK_GRID( page ), but, 0, row, narrow, 1 );

        // g_signal_connect(  );
    }
    ++row;
}


void settings_free( settings_t* settings )
{
    free( settings->pages );
    settings->pages = NULL;
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


void add_entry( GtkButton* self, void* ptr ) {}
void remove_entry( GtkButton* self, void* ptr ) {}

void set_monitor( GtkComboBox* self, page_t* ptr )
{
    int active = gtk_combo_box_get_active( self );

    switch ( active )
    {
        // case 0: ptr->entry->section->collector = 
        default: return;
    }
}


void set_interval( GtkSpinButton* self, page_t* ptr )
{
    int ms = read_spin( self );
    if ( ms > 0 )
        entry_set_interval( ptr->entry, ms );
}


void set_graph_mode( GtkComboBox* self, page_t* ptr )
{
    switch ( gtk_combo_box_get_active( self ) )
    {
        case 0: ptr->entry->section->draw_func = &draw_lin; break;
        case 1: ptr->entry->section->draw_func = &draw_led; break;
        default: return;
    }
}

void toggle_label( GtkCheckButton* self, page_t* ptr ) {}
void set_label( GtkEntry* self, page_t* ptr ) {}


static void rgba_to_array( GdkRGBA* rgba, double* out )
{
    out[ 0 ] = rgba->red;
    out[ 1 ] = rgba->green;
    out[ 2 ] = rgba->blue;
    // TODO: add alpha
}

void set_primary_color( GtkColorButton* self, page_t* ptr )
{
    GdkRGBA rgba = { 0 };
    gtk_color_chooser_get_rgba( GTK_COLOR_CHOOSER( self ), &rgba );
    rgba_to_array( &rgba, ptr->entry->section->graph.rgb_on );
}

void set_secondary_color( GtkColorButton* self, page_t* ptr )
{
    GdkRGBA rgba = { 0 };
    gtk_color_chooser_get_rgba( GTK_COLOR_CHOOSER( self ), &rgba );
    rgba_to_array( &rgba, ptr->entry->section->graph.rgb_off );
}


void set_graph_w( GtkSpinButton* self, page_t* ptr )
{
    ptr->entry->section->graph.w = read_spin( self );
    entry_refresh( ptr->entry );
}

void set_graph_h( GtkSpinButton* self, page_t* ptr )
{
    ptr->entry->section->graph.h = read_spin( self );
    entry_refresh( ptr->entry );
}

void set_graph_blk_w( GtkSpinButton* self, page_t* ptr )
{
    ptr->entry->section->graph.blk_w = read_spin( self );
    entry_refresh( ptr->entry );
}

void set_graph_blk_h( GtkSpinButton* self, page_t* ptr )
{
    ptr->entry->section->graph.blk_h = read_spin( self );
    entry_refresh( ptr->entry );
}

void set_graph_pad_x( GtkSpinButton* self, page_t* ptr )
{
    ptr->entry->section->graph.pad_x = read_spin( self );
    entry_refresh( ptr->entry );
}

void set_graph_pad_y( GtkSpinButton* self, page_t* ptr )
{
    ptr->entry->section->graph.pad_y = read_spin( self );
    entry_refresh( ptr->entry );
}

