//----------------------------------------------------------------------------
// \file    types.h
// \brief   .
//
//----------------------------------------------------------------------------

#include <stdint.h>
#define TYPES_H  //hack to not use this


#ifndef TYPES_H
#define TYPES_H

//----------------------------------------------------------------------------
// datatypes
//----------------------------------------------------------------------------
typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;
typedef unsigned long   ulong_t;
typedef signed char     int8_t;
typedef signed short    int16_t;
typedef signed int      int32_t;
typedef signed long     long_t;

typedef enum 
{
	false = 0,
	true = 1
}bool_e; 

#endif
