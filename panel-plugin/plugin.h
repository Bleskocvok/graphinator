#pragma once

#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include "graphinator.h"

#include <libxfce4panel/libxfce4panel.h>  // XfcePanelPlugin


void plugin_save( XfcePanelPlugin* panel, const panel_t* data );
void plugin_load( XfcePanelPlugin* panel, panel_t* data );


#endif // __PLUGIN_H__
