
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


static void combo_add_options( GtkComboBoxText* combo,
                               const char** opts,
                               size_t count )
{
    for ( size_t i = 0; i < count; i++ )
        gtk_combo_box_text_append_text( combo, opts[ i ] );
}


static GtkWidget* add_label( GtkWidget* page,
                             const char* txt,
                             int col, int row,
                             int grid_w, int grid_h,
                             int font_h,
                             double xalign )
{
    GtkWidget* label = gtk_label_new( txt );
    gtk_widget_show( label );
    gtk_label_set_xalign( GTK_LABEL( label ), xalign );
    gtk_widget_set_size_request( label, -1, font_h );
    gtk_grid_attach( GTK_GRID( page ), label, col, row, grid_w, grid_h );
    return label;
}


static GtkWidget* create_combo_box( const char** opts, size_t count,
                                    int selected )
{
    GtkWidget* result = gtk_combo_box_text_new();
    gtk_widget_show( result );
    combo_add_options( GTK_COMBO_BOX_TEXT( result ), opts, count );
    gtk_combo_box_set_active( GTK_COMBO_BOX( result ), selected );
    return result;
}


static GtkWidget* create_color_chooser( double r, double g, double b,
                                        double a )
{
    GdkRGBA rgba = { .red = r, .green = g, .blue = b, .alpha = a };
    GtkWidget* color_chooser = gtk_color_button_new_with_rgba( &rgba );
    gtk_widget_show( color_chooser );
    return color_chooser;
}


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

    const int    font_h = 24;
    const int    wide   = 4;
    const int    narrow = 2;
    const double xalign = 1;
    int row = 0;

    add_label( page, "Monitor:", 0, row, wide, 1, font_h, xalign );

    static const char* monitor_opts[] = { "CPU", "Memory", "Custom" };
    ctx->combo_monitor = create_combo_box( monitor_opts, 3, 0 );
    gtk_grid_attach( GTK_GRID( page ), ctx->combo_monitor, wide, row, wide, 1 );

    ++row;

    add_label( page, "Update interval (ms):", 0, row, wide, 1, font_h, xalign );

    ctx->spin_interval = gtk_spin_button_new_with_range( 0, 9999999, 1 );
    gtk_widget_show( ctx->spin_interval );
    gtk_grid_attach( GTK_GRID( page ), ctx->spin_interval, wide, row, wide, 1 );

    ++row;

    add_label( page, "Graph mode:", 0, row, wide, 1, font_h, xalign );

    static const char* graph_opts[] = { "Normal", "LED" };
    ctx->combo_graph = create_combo_box( graph_opts, 2, 0 );
    gtk_grid_attach( GTK_GRID( page ), ctx->combo_graph, wide, row, wide, 1 );

    ++row;

    ctx->check_label = gtk_check_button_new_with_label( "Show label" );
    gtk_widget_show( ctx->check_label );
    gtk_grid_attach( GTK_GRID( page ), ctx->check_label, 0, row, wide, 1 );
    gtk_widget_set_halign( ctx->check_label, GTK_ALIGN_END );

    ctx->entry_label = gtk_entry_new();
    gtk_widget_show( ctx->entry_label );
    gtk_entry_set_placeholder_text( GTK_ENTRY( ctx->entry_label ), "Label" );
    gtk_widget_set_sensitive( ctx->entry_label, FALSE );
    gtk_grid_attach( GTK_GRID( page ), ctx->entry_label, wide, row, wide, 1 );
    // g_signal_connect(  );

    ++row;

    add_label( page, "Primary color:", 0, row, wide, 1, font_h, xalign );

    ctx->color_primary = create_color_chooser( 0, 0, 0, 1 );
    gtk_grid_attach( GTK_GRID( page ), ctx->color_primary, wide, row, wide, 1 );

    ++row;

    add_label( page, "Secondary color:", 0, row, wide, 1, font_h, xalign );

    ctx->color_secondary = create_color_chooser( 1, 0, 1, 1 );
    gtk_grid_attach( GTK_GRID( page ), ctx->color_secondary, wide, row, wide, 1 );

    ++row;

    add_label( page, u8"Width ⨯ Height:", 0, row, wide, 1, font_h, xalign );

    ctx->spin_w = gtk_spin_button_new_with_range( 0, 9999, 1 );
    gtk_widget_show( ctx->spin_w );
    gtk_grid_attach( GTK_GRID( page ), ctx->spin_w, wide, row, narrow, 1 );

    ctx->spin_h = gtk_spin_button_new_with_range( 0, 9999, 1 );
    gtk_widget_show( ctx->spin_h );
    gtk_grid_attach( GTK_GRID( page ), ctx->spin_h, wide + narrow, row, narrow, 1 );

    ++row;

    add_label( page, u8"Block Width ⨯ Block Height:", 0, row, wide, 1, font_h, xalign );

    ctx->spin_blk_w = gtk_spin_button_new_with_range( 0, 9999, 1 );
    gtk_widget_show( ctx->spin_blk_w );
    gtk_grid_attach( GTK_GRID( page ), ctx->spin_blk_w, wide, row, narrow, 1 );

    ctx->spin_blk_h = gtk_spin_button_new_with_range( 0, 9999, 1 );
    gtk_widget_show( ctx->spin_blk_h );
    gtk_grid_attach( GTK_GRID( page ), ctx->spin_blk_h, wide + narrow, row, narrow, 1 );

    ++row;

    add_label( page, u8"Spacing X ⨯ Spacing Y:", 0, row, wide, 1, font_h, xalign );

    ctx->spin_pad_x = gtk_spin_button_new_with_range( 0, 9999, 1 );
    gtk_widget_show( ctx->spin_pad_x );
    gtk_grid_attach( GTK_GRID( page ), ctx->spin_pad_x, wide, row, narrow, 1 );

    ctx->spin_pad_y = gtk_spin_button_new_with_range( 0, 9999, 1 );
    gtk_widget_show( ctx->spin_pad_y );
    gtk_grid_attach( GTK_GRID( page ), ctx->spin_pad_y, wide + narrow,
                                        row, narrow, 1 );

    ++row;

    {
        GtkWidget* but_remove = gtk_button_new_with_label( "Remove section" );
        gtk_widget_show( but_remove );
        gtk_grid_attach( GTK_GRID( page ), but_remove, 0, row, narrow, 1 );
        // g_signal_connect(  );
    }

    ++row;

    g_signal_connect( ctx->combo_monitor, "changed",
                                        G_CALLBACK( set_monitor ), ctx );
    g_signal_connect( ctx->spin_interval, "value-changed",
                                        G_CALLBACK( set_interval ), ctx );
    g_signal_connect( ctx->combo_graph, "changed",
                                        G_CALLBACK( set_graph_mode ), ctx );
    g_signal_connect( ctx->color_primary, "color-set",
                                        G_CALLBACK( set_primary_color ), ctx );
    g_signal_connect( ctx->color_secondary, "color-set",
                                        G_CALLBACK( set_secondary_color ), ctx );
    g_signal_connect( ctx->spin_w, "value-changed",
                                        G_CALLBACK( set_graph_w ), ctx );
    g_signal_connect( ctx->spin_h, "value-changed",
                                        G_CALLBACK( set_graph_h ), ctx );
    g_signal_connect( ctx->spin_blk_w, "value-changed",
                                        G_CALLBACK( set_graph_blk_w ), ctx );
    g_signal_connect( ctx->spin_blk_h, "value-changed",
                                        G_CALLBACK( set_graph_blk_h ), ctx );
    g_signal_connect( ctx->spin_pad_x, "value-changed",
                                        G_CALLBACK( set_graph_pad_x ), ctx );
    g_signal_connect( ctx->spin_pad_y, "value-changed",
                                        G_CALLBACK( set_graph_pad_y ), ctx );
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

    gtk_notebook_set_current_page( notebook, 0 );
}


// TODO
void add_entry( GtkButton* self, void* ptr ) {}
// TODO
void remove_entry( GtkButton* self, void* ptr ) {}


void set_monitor( GtkComboBox* self, page_t* ptr )
{
    int active = gtk_combo_box_get_active( self );

    switch ( active )
    {
        case 0:
            collector_reset( &ptr->entry->section->collector, init_cpu_data,
                                                              collect_cpu_data,
                                                              free_cpu_data );
            break;
        case 1:
            collector_reset( &ptr->entry->section->collector, init_mem_data,
                                                              collect_mem_data,
                                                              free_mem_data );
            break;
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

// TODO
void toggle_label( GtkCheckButton* self, page_t* ptr ) {}
// TODO
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

