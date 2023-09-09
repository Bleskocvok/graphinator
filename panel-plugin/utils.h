#pragma once

#ifndef __UTILS_H__
#define __UTILS_H__


#define  M_COUNT( x )   ( sizeof( x ) / sizeof( *x ) )

#define  M_MIN( x, y )  ( ( ( x ) > ( y ) ) ? ( y ) : ( x ) )

#define  M_MAX( x, y )  ( ( ( x ) > ( y ) ) ? ( x ) : ( y ) )


// if C11
#if __STDC_VERSION__ == 201112L
#define  M_U8  u8""
#else
#define  M_U8  ( const char* )
#endif


#endif // __UTILS_H__
