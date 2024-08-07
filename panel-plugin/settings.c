
#include "settings.h"

#include "utils.h"      // M_COUNT, M_U8, M_MAX
#include "collector.h"
#include "graphinator.h"

#include <stdlib.h>     // NULL, calloc
#include <string.h>     // strncat


//
// Forward declarations
//

// Callbacks
void add_entry(         GtkButton*      self,   void*   ptr  );
void remove_entry(      GtkButton*      self,   void*   ptr  );
void set_monitor(       GtkComboBox*    self,   page_t* ptr  );
void set_interval(      GtkSpinButton*  self,   page_t* ptr  );
void set_graph_mode(    GtkComboBox*    self,   page_t* ptr  );
void toggle_label(      GtkCheckButton* self,   page_t* ptr  );
void set_label(         GtkEntry*       self,   page_t* ptr  );
void set_prim_color(    GtkColorButton* self,   page_t* ptr  );
void set_secon_color(   GtkColorButton* self,   page_t* ptr  );
void set_w(             GtkSpinButton*  self,   page_t* ptr  );
void set_h(             GtkSpinButton*  self,   page_t* ptr  );
void set_blk_w(         GtkSpinButton*  self,   page_t* ptr  );
void set_blk_h(         GtkSpinButton*  self,   page_t* ptr  );
void set_pad_x(         GtkSpinButton*  self,   page_t* ptr  );
void set_pad_y(         GtkSpinButton*  self,   page_t* ptr  );


// Main

void add_page( GtkNotebook* notebook, page_t* ctx );
void page_set_current_settings( page_t* ctx );
void page_setup_signals( page_t* p );

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
    GtkComboBoxText* combo = GTK_COMBO_BOX_TEXT( result );
    for ( size_t i = 0; i < count; i++ )
        gtk_combo_box_text_append_text( combo, opts[ i ] );
    gtk_combo_box_set_active( GTK_COMBO_BOX( combo ), selected );
    return result;
}


static GtkWidget* create_color_button( double r, double g, double b,
                                       double a )
{
    GdkRGBA rgba = { .red = r, .green = g, .blue = b, .alpha = a };
    GtkWidget* color_chooser = gtk_color_button_new_with_rgba( &rgba );
    gtk_widget_show( color_chooser );
    return color_chooser;
}


static void rgba_to_array( GdkRGBA* rgba, double* out )
{
    out[ 0 ] = rgba->red;
    out[ 1 ] = rgba->green;
    out[ 2 ] = rgba->blue;
    // TODO: add alpha
}


static GdkRGBA array_to_rgba( double* arr )
{
    GdkRGBA rgba = (GdkRGBA)
    {
        .red   = arr[ 0 ],
        .green = arr[ 1 ],
        .blue  = arr[ 2 ],
        .alpha = 1,
        // TODO: read alpha from arr
    };
    return rgba;
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

    const int    font_h = 24;
    const int    wide   = 4;
    const int    narrow = 2;
    const double xalign = 1;
    int row = 0;

    add_label( page, "Monitor:", 0, row, wide, 1, font_h, xalign );

    ctx->combo_mon = create_combo_box( MONITORS_STR, MONITORS_COUNT, 0 );
    gtk_grid_attach( GTK_GRID( page ), ctx->combo_mon, wide, row, wide, 1 );

    ++row;

    add_label( page, "Update interval (ms):", 0, row, wide, 1, font_h, xalign );

    ctx->spin_inter = gtk_spin_button_new_with_range( 0, 9999999, 1 );
    gtk_widget_show( ctx->spin_inter );
    gtk_grid_attach( GTK_GRID( page ), ctx->spin_inter, wide, row, wide, 1 );

    ++row;

    add_label( page, "Graph mode:", 0, row, wide, 1, font_h, xalign );

    ctx->combo_graph = create_combo_box( GRAPHS_STR, M_COUNT( GRAPHS_STR ), 0 );
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

    ++row;

    add_label( page, "Main colors:", 0, row, wide, 1,
               font_h, xalign );

    ctx->color_prim = create_color_button( 0, 0, 0, 1 );
    gtk_grid_attach( GTK_GRID( page ), ctx->color_prim, wide, row,
                                                        narrow, 1 );

    ctx->color_secon = create_color_button( 1, 0, 1, 1 );
    gtk_grid_attach( GTK_GRID( page ), ctx->color_secon, wide + narrow, row,
                                                         narrow, 1 );

    ++row;

    add_label( page, M_U8"Width ⨯ Height:", 0, row, wide, 1, font_h, xalign );

    ctx->spin_w = gtk_spin_button_new_with_range( 0, 9999, 1 );
    gtk_widget_show( ctx->spin_w );
    gtk_grid_attach( GTK_GRID( page ), ctx->spin_w, wide, row, narrow, 1 );

    ctx->spin_h = gtk_spin_button_new_with_range( 0, 9999, 1 );
    gtk_widget_show( ctx->spin_h );
    gtk_grid_attach( GTK_GRID( page ), ctx->spin_h, wide + narrow, row,
                                       narrow, 1 );

    ++row;

    add_label( page, M_U8"Block width ⨯ Block height:", 0, row, wide, 1, font_h,
               xalign );

    ctx->spin_blk_w = gtk_spin_button_new_with_range( 0, 9999, 1 );
    gtk_widget_show( ctx->spin_blk_w );
    gtk_grid_attach( GTK_GRID( page ), ctx->spin_blk_w, wide, row, narrow, 1 );

    ctx->spin_blk_h = gtk_spin_button_new_with_range( 0, 9999, 1 );
    gtk_widget_show( ctx->spin_blk_h );
    gtk_grid_attach( GTK_GRID( page ), ctx->spin_blk_h, wide + narrow, row,
                                       narrow, 1 );

    ++row;

    add_label( page, M_U8"Spacing X ⨯ Spacing Y:", 0, row, wide, 1, font_h,
               xalign );

    ctx->spin_pad_x = gtk_spin_button_new_with_range( 0, 9999, 1 );
    gtk_widget_show( ctx->spin_pad_x );
    gtk_grid_attach( GTK_GRID( page ), ctx->spin_pad_x, wide, row, narrow, 1 );

    ctx->spin_pad_y = gtk_spin_button_new_with_range( 0, 9999, 1 );
    gtk_widget_show( ctx->spin_pad_y );
    gtk_grid_attach( GTK_GRID( page ), ctx->spin_pad_y, wide + narrow,
                                        row, narrow, 1 );

    ++row;

    // // Remove section
    // GtkWidget* but_remove = gtk_button_new_with_label( "Remove section" );
    // gtk_widget_show( but_remove );
    // gtk_grid_attach( GTK_GRID( page ), but_remove, 0, row, narrow, 1 );
    // // g_signal_connect(  );
    // ++row;

    page_setup_signals( ctx );

    page_set_current_settings( ctx );
}


void page_setup_signals( page_t* p )
{
#define  connect( obj, str, call, p )  \
    g_signal_connect( obj, str, G_CALLBACK( call ), p )

    connect( p->combo_mon,   "changed",         set_monitor,     p );
    connect( p->spin_inter,  "value-changed",   set_interval,    p );
    connect( p->combo_graph, "changed",         set_graph_mode,  p );
    connect( p->check_label, "toggled",         toggle_label,    p );
    connect( p->color_prim,  "color-set",       set_prim_color,  p );
    connect( p->color_secon, "color-set",       set_secon_color, p );
    connect( p->spin_w,      "value-changed",   set_w,           p );
    connect( p->spin_h,      "value-changed",   set_h,           p );
    connect( p->spin_blk_w,  "value-changed",   set_blk_w,       p );
    connect( p->spin_blk_h,  "value-changed",   set_blk_h,       p );
    connect( p->spin_pad_x,  "value-changed",   set_pad_x,       p );
    connect( p->spin_pad_y,  "value-changed",   set_pad_y,       p );
    // entry signals
    connect( p->entry_label, "activate",        set_label,       p );
    connect( p->entry_label, "changed",         set_label,       p );

#undef  connect
}


void page_set_current_settings( page_t* p )
{
    int i_mon   = find_collector( MONITORS_COL, MONITORS_COUNT,
                                  &p->entry->section->collector );

    int i_graph = find_draw_func( GRAPHS_FUNC, GRAPHS_COUNT,
                                  p->entry->section->draw_func );

    gtk_combo_box_set_active( GTK_COMBO_BOX( p->combo_mon ),   i_mon   );
    gtk_combo_box_set_active( GTK_COMBO_BOX( p->combo_graph ), i_graph );

    gtk_spin_button_set_value( GTK_SPIN_BUTTON( p->spin_inter ),
                               p->entry->section->interval );

    {
        GdkRGBA c = array_to_rgba( p->entry->section->graph.rgb_on );
        gtk_color_chooser_set_rgba( GTK_COLOR_CHOOSER( p->color_prim ), &c );
    }
    {
        GdkRGBA c = array_to_rgba( p->entry->section->graph.rgb_off );
        gtk_color_chooser_set_rgba( GTK_COLOR_CHOOSER( p->color_secon ), &c );
    }

    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( p->check_label ),
                                  p->entry->section->label_enabled );

    gtk_entry_set_text( GTK_ENTRY( p->entry_label ),
                                   p->entry->section->label_str );

    gtk_spin_button_set_value( GTK_SPIN_BUTTON( p->spin_w ),
                               p->entry->section->graph.w );
    gtk_spin_button_set_value( GTK_SPIN_BUTTON( p->spin_h ),
                               p->entry->section->graph.h );
    gtk_spin_button_set_value( GTK_SPIN_BUTTON( p->spin_blk_w ),
                               p->entry->section->graph.blk_w );
    gtk_spin_button_set_value( GTK_SPIN_BUTTON( p->spin_blk_h ),
                               p->entry->section->graph.blk_h );
    gtk_spin_button_set_value( GTK_SPIN_BUTTON( p->spin_pad_x ),
                               p->entry->section->graph.pad_x );
    gtk_spin_button_set_value( GTK_SPIN_BUTTON( p->spin_pad_y ),
                               p->entry->section->graph.pad_y );
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
    // // Add section
    // GtkWidget* new_but = gtk_button_new_with_label( "Add section" );
    // gtk_widget_show( new_but );
    // gtk_container_add( GTK_CONTAINER( container ), new_but );
    // // Add section signal
    // g_signal_connect( new_but, "clicked", G_CALLBACK( add_entry ), NULL );

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
        settings->pages[ i ].settings = settings;
        add_page( notebook, &settings->pages[ i ] );
    }

    gtk_notebook_set_current_page( notebook, 0 );
}


// TODO
void add_entry( GtkButton* self, void* ptr )
{
    (void) self;
    (void) ptr;
}

// TODO
void remove_entry( GtkButton* self, void* ptr )
{
    (void) self;
    (void) ptr;
}


void set_monitor( GtkComboBox* self, page_t* ptr )
{
    int active = gtk_combo_box_get_active( self );

    if ( active >= MONITORS_COUNT )
        return;

    collector_t col = *MONITORS_COL[ active ];
    collector_reset( &ptr->entry->section->collector, &col );
}


void set_interval( GtkSpinButton* self, page_t* ptr )
{
    int ms = read_spin( self );
    if ( ms > 0 )
        entry_set_interval( ptr->entry, ms );
}


void set_graph_mode( GtkComboBox* self, page_t* ptr )
{
    int idx = gtk_combo_box_get_active( self );
    if ( idx >= GRAPHS_COUNT )
        return;

    ptr->entry->section->draw_func = GRAPHS_FUNC[ idx ];

    // switch ( gtk_combo_box_get_active( self ) )
    // {
    //     case 0: ptr->entry->section->draw_func = &draw_lin; break;
    //     case 1: ptr->entry->section->draw_func = &draw_led; break;
    //     default: return;
    // }
}


void toggle_label( GtkCheckButton* self, page_t* ptr )
{
    int b = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( self ) );
    gtk_widget_set_sensitive( ptr->entry_label, b );

    ptr->entry->section->label_enabled = b;
    entry_refresh_label( ptr->entry );

    entries_refresh_all( ptr->settings->entries );
}


void set_label( GtkEntry* self, page_t* ptr )
{
    const char* text = gtk_entry_get_text( self );
    ptr->entry->section->label_str[ 0 ] = '\0';
    strncat( ptr->entry->section->label_str, text, LABEL_MAX_LEN - 1 );
    entry_refresh_label( ptr->entry );
}


void set_prim_color( GtkColorButton* self, page_t* ptr )
{
    GdkRGBA rgba = { 0 };
    gtk_color_chooser_get_rgba( GTK_COLOR_CHOOSER( self ), &rgba );
    rgba_to_array( &rgba, ptr->entry->section->graph.rgb_on );
}

void set_secon_color( GtkColorButton* self, page_t* ptr )
{
    GdkRGBA rgba = { 0 };
    gtk_color_chooser_get_rgba( GTK_COLOR_CHOOSER( self ), &rgba );
    rgba_to_array( &rgba, ptr->entry->section->graph.rgb_off );
}


void set_w( GtkSpinButton* self, page_t* ptr )
{
    ptr->entry->section->graph.w = read_spin( self );

    // workaround to avoid crash with poorly set width
    static const int MIN_WIDTH = 5;
    ptr->entry->section->graph.w = M_MAX( MIN_WIDTH,
                                          ptr->entry->section->graph.w );
    // TODO: figure out the underlying issue with width ~ 0
    // (appears to crash for width ∈ { 0, 1, 2 })

    entry_refresh( ptr->entry );
}

void set_h( GtkSpinButton* self, page_t* ptr )
{
    ptr->entry->section->graph.h = read_spin( self );
    entry_refresh( ptr->entry );
}

void set_blk_w( GtkSpinButton* self, page_t* ptr )
{
    ptr->entry->section->graph.blk_w = read_spin( self );
    entry_refresh( ptr->entry );
}

void set_blk_h( GtkSpinButton* self, page_t* ptr )
{
    ptr->entry->section->graph.blk_h = read_spin( self );
    entry_refresh( ptr->entry );
}

void set_pad_x( GtkSpinButton* self, page_t* ptr )
{
    ptr->entry->section->graph.pad_x = read_spin( self );
    entry_refresh( ptr->entry );
}

void set_pad_y( GtkSpinButton* self, page_t* ptr )
{
    ptr->entry->section->graph.pad_y = read_spin( self );
    entry_refresh( ptr->entry );
}

