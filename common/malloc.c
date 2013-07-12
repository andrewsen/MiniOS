 /* $Header: /opt/proj/minix/cvsroot/src/lib/ansi/malloc.c,v 1.1.1.1 2005/04/21 14:56:05 beng Exp $ */
 
 /* replace undef by define */
 #undef   DEBUG          /* check assertions */
 #undef   SLOWDEBUG      /* some extra test loops (requires DEBUG) */
 
 #ifndef DEBUG
 #define NDEBUG
 #endif
 
 #include        "stdlib.h"
 #include        "string.h"
 #include        "errno.h"
 #include        "assert.h"
#include "heap.h"

 extern heap_t *kheap;

 #if _EM_WSIZE == _EM_PSIZE
 #define ptrint          int
 #else
 #define ptrint          long
 #endif
 
 #if     _EM_PSIZE == 2
 #define BRKSIZE         1024
 #else
 #define BRKSIZE         4096
 #endif
 #define PTRSIZE         ((int) sizeof(void *))
 #define Align(x,a)      (((x) + (a - 1)) & ~(a - 1))
 #define NextSlot(p)     (* (void **) ((p) - PTRSIZE))
 #define NextFree(p)     (* (void **) (p))
 
 /*
  * A short explanation of the data structure and algorithms.
  * An area returned by kmalloc() is called a slot. Each slot
  * contains the number of bytes requested, but preceeded by
  * an extra pointer to the next the slot in memory.
  * '_bottom' and '_top' point to the first/last slot.
  * More memory is asked for using brk() and appended to top.
  * The list of free slots is maintained to keep kmalloc() fast.
  * '_empty' points the the first free slot. Free slots are
  * linked together by a pointer at the start of the
  * user visable part, so just after the next-slot pointer.
  * Free slots are merged together by free().
  */
 
 //extern void *_sbrk(int);
 //extern int _brk(void *);
 static void *_bottom, *_top, *_empty;
 
 void setKmmBottom(void * b) {
     b = Align((ptrint)b, PTRSIZE);
     b += PTRSIZE;
     _top = _bottom = b;
     NextSlot(b) = 0;
 }

 static int grow(size_t len)
 {
   register char *p;
 
   assert(NextSlot((char *)_top) == 0);
   if ((char *) _top + len < (char *) _top
       || (p = (char *)Align((ptrint)_top + len, BRKSIZE)) < (char *) _top ) {
         errno = ENOMEM;
         return(0);
   }
   //if (_brk(p) != 0)
   //      return(0);
   NextSlot((char *)_top) = p;
   NextSlot(p) = 0;
   kfree(_top);
   _top = p;
   return 1;
 }

 void * kmalloc_ap(size_t size, uint32_t * phys) {
     *phys = (uint32_t)kmalloc(size);
     if (*phys & 0xFFFFF000) {
        *phys &= 0xFFFFF000;
        *phys += 0x1000;
     }
     return (void*)*phys;
 }
 void * kmalloc_a(size_t size) {
     uint32_t phys;
     phys = (uint32_t)kmalloc(size);
     if (phys & 0xFFFFF000) {
        phys &= 0xFFFFF000;
        phys += 0x1000;
     }
     return (void*)phys;
 }

 void *
 kmalloc(size_t size) //Align _sbrk NextSlot assert? NextFree
 {
     if(kheap != 0) {
         return alloc(size, 1, kheap);
     }
   register char *prev, *p, *next, *_new;
   register unsigned len, ntries;
 
   if (size == 0)
         return NULL;
 
   for (ntries = 0; ntries < 2; ntries++) {
         if ((len = Align(size, PTRSIZE) + PTRSIZE) < 2 * PTRSIZE) {
                 errno = ENOMEM;
                 return NULL;
         }
         if (_bottom == 0) {
                 //if ((p = _sbrk(2 * PTRSIZE)) == (char *) -1)
                 //        return NULL;
                 p = (char *) Align((ptrint)p, PTRSIZE);
                 p += PTRSIZE;
                 _top = _bottom = p;
                 NextSlot(p) = 0;
         }
 #ifdef SLOWDEBUG
         for (p = _bottom; (next = NextSlot(p)) != 0; p = next)
                 assert(next > p);
         assert(p == _top);
 #endif
         for (prev = 0, p = _empty; p != 0; prev = p, p = NextFree(p)) {
                 next = NextSlot(p);
                 _new = p + len;  /* easily overflows!! */
                 if (_new > next || _new <= p)
                         continue;               /* too small */
                 if (_new + PTRSIZE < next) {     /* too big, so split */
                         /* + PTRSIZE avoids tiny slots on free list */
                         NextSlot(_new) = next;
                         NextSlot(p) = _new;
                         NextFree(_new) = NextFree(p);
                         NextFree(p) = _new;
                 }
                 if (prev)
                         NextFree(prev) = NextFree(p);
                 else
                         _empty = NextFree(p);
                 return p;
         }
         if (grow(len) == 0)
                 break;
   }
   assert(ntries != 2);
   return NULL;
 }
 
 void *
 realloc(void *oldp, size_t size)
 {
   register char *prev, *p, *next, *_new;
   char *old = oldp;
   register size_t len, n;
 
   if (old == 0)
         return kmalloc(size);
   if (size == 0) {
         kfree(old);
         return NULL;
   }
   len = Align(size, PTRSIZE) + PTRSIZE;
   next = NextSlot(old);
   n = (int)(next - old);                        /* old length */
   /*
    * extend old if there is any free space just behind it
    */
   for (prev = 0, p = _empty; p != 0; prev = p, p = NextFree(p)) {
         if (p > next)
                 break;
         if (p == next) {        /* 'next' is a free slot: merge */
                 NextSlot(old) = NextSlot(p);
                 if (prev)
                         NextFree(prev) = NextFree(p);
                 else
                         _empty = NextFree(p);
                 next = NextSlot(old);
                 break;
         }
   }
   _new = old + len;
  /*
    * Can we use the old, possibly extended slot?
    */
   if (_new <= next && _new >= old) {              /* it does fit */
         if (_new + PTRSIZE < next) {             /* too big, so split */
                 /* + PTRSIZE avoids tiny slots on free list */
                 NextSlot(_new) = next;
                 NextSlot(old) = _new;
                 kfree(_new);
         }
         return old;
   }
   if ((_new = kmalloc(size)) == NULL)             /* it didn't fit */
         return NULL;
   memcpy(_new, old, n);                          /* n < size */
   kfree(old);
   return _new;
 }
 
 void
 kfree(void *ptr)
 {

     if(kheap != 0) {
         free(ptr, kheap);
         return;
         //return alloc(size, 1, kheap);
     }
   register char *prev, *next;
   char *p = ptr;
 
   if (p == 0)
         return;
 
   assert(NextSlot(p) > p);
   for (prev = 0, next = _empty; next != 0; prev = next, next = NextFree(next))
         if (p < next)
                 break;
   NextFree(p) = next;
   if (prev)
         NextFree(prev) = p;
   else
         _empty = p;
   if (next) {
         assert(NextSlot(p) <= next);
         if (NextSlot(p) == next) {              /* merge p and next */
                 NextSlot(p) = NextSlot(next);
                 NextFree(p) = NextFree(next);
         }
   }
   if (prev) {
         assert(NextSlot(prev) <= p);
         if (NextSlot(prev) == p) {              /* merge prev and p */
                 NextSlot(prev) = NextSlot(p);
                 NextFree(prev) = NextFree(p);
         }
   }
 }
