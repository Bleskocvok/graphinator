#pragma once

#ifndef __DEFAULTS_H__
#define __DEFAULTS_H__

// custom
#include "graphinator.h"


#define M_COUNT( x )  ( sizeof( x ) / sizeof( *x ) )


extern section_t default_sections[];
extern size_t default_sections_count;


#endif // __DEFAULTS_H__
