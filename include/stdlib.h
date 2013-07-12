#ifndef STDLIB_H
#define STDLIB_H

#include "types.h"
#include "printf.h"
#include "scanf.h"
#include "screen.h"
#include "io.h"

#ifdef __NO_PAGING
#include "heap.h"

/*
#define HEAP_START 0xD0000000
#define HEAP_END   0xFFBFF000

typedef struct header
{
  struct header *prev, *next;
  uint32_t allocated : 1;
  uint32_t length : 31;
} header_t;

void init_heap ();

// Returns a pointer to a chunk of memory, minimum size 'l' bytes.
void *malloc (uint32_t l);

// Takes a chunk of memory allocated with kmalloc, and returns it to the pool of usable memory.
void free (void *p);
*/
// void * malloc(size_t size);

#define kmalloc_p(sz, phys) kmalloc(sz)

#define kmalloc_1(sz, phys) kmalloc(sz)

#define kmalloc_2(sz, a, phys) kmalloc(sz)
 //void free(void *ptr);
void* kmalloc(size_t sz);
void kfree(void* p);

//uint32_t kmalloc_1(uint32_t sz, int align);

//uint32_t kmalloc_2(uint32_t sz, int align, uint32_t *phys);
 void setKmmBottom(void * b);

 void * kmalloc_ap(size_t size, uint32_t * phys);

 void * kmalloc_a(size_t size);


//#define malloc kmalloc
//#define kfree free

#endif
#endif
