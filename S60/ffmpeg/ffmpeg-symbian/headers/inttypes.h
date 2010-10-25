#ifndef __INTTYPES_H
#define __INTTYPES_H

#include <stddef.h>

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;

typedef unsigned long long uint64_t;
typedef signed long long int64_t;

typedef int8_t int_fast8_t;
typedef int16_t int_fast16_t;
typedef int32_t int_fast32_t;
typedef int64_t int_fast64_t;
typedef uint8_t uint_fast8_t;
typedef uint16_t uint_fast16_t;
typedef uint32_t uint_fast32_t;
typedef uint64_t uint_fast64_t;

typedef long intptr_t;

/*
typedef struct {
	uint32_t low;
	uint32_t high;
} uint64_t;

typedef struct {
	uint32_t low;
	int32_t high;
} int64_t;
*/

/*
#include <e32base.h>

typedef TUInt64 uint64_t;
typedef TInt64 int64_t;
*/


#endif
