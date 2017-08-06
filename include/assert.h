/*
assert.h

Copyright 1995 Philip Homburg
*/
#ifndef INET_ASSERT_H
#define INET_ASSERT_H
#define NDEBUG 0
#if !NDEBUG
#include "printf.h"
#include "types.h"
void bad_assertion(char *file, int line, char *what);

#define assert(x)       ((void)(!(x) ? bad_assertion(__FILE__, __LINE__, #x),0 : 0))
#define ASSERT(x) (assert(x))
#define PANIC(x)       (printf("%s %s %s", (x),  __FILE__, __LINE__));

void panic(const char *text, int level, int32_t data);
//#define panic(x, y) (panic(x, y, -1))

#else /* NDEBUG */

#define assert(x)               0
#define compare(a,t,b)          0

#endif /* NDEBUG */

 #endif /* INET_ASSERT_H */
 
 
 /*
  * $PchId: assert.h,v 1.8 2002/03/18 21:50:32 philip Exp $
  */
