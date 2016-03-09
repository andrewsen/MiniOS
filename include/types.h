#ifndef _TYPES_H
#define _TYPES_H

#define NULL 0
#define TRUE 1
#define FALSE 0
#define asm __asm
#define ASM __asm
#define ASMV __asm volatile

#define ALIGNED(n) __attribute__((aligned(n)))
#define NOINLINE __attribute__((noinline))
#define PACKED __attribute__((packed))
#define SECTION(name) __attribute__((section(name)))
#define UNUSED __attribute__((unused))

#define CODE_SEGMENT 8

typedef unsigned int size_t;
typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef char s8;

#ifndef __cplusplus
#define true TRUE
#define false FALSE
typedef unsigned int bool;
#endif

typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
//#ifdef __LP64__
//typedef long int64_t;
//typedef unsigned long uint64_t;
//typedef unsigned long size_t;
//#else
typedef long long int64_t;
typedef unsigned long long uint64_t;
typedef unsigned int size_t;

typedef uint8_t byte;
typedef uint16_t word;
typedef uint32_t dword;

/**DISABLE PAGING**/
#define __NO_PAGING
/******************/

#endif
