//#if CHAPTER >= 8
// 
// heap.h -- Declares functions and structures for the kernel heap.
//           Written for JamesM's kernel development tutorials.
//

#ifndef HEAP_H
#define HEAP_H

#include "types.h"
#include "ordered_array.h"

#define KHEAP_START         0xC0000000
#define KHEAP_INITIAL_SIZE  0x100000
#define HEAP_INDEX_SIZE   0x20000
#define HEAP_MAGIC        0x123890AB
#define HEAP_MIN_SIZE     0x70000

/**
  Информация о размере неиспользуемого фрагмента/используемого блока памяти
**/
typedef struct
{
   uint32_t magic;   // Магическое число, используемое для контроля ошибок и индентификации.
   uint8_t is_hole;   // 1 — если это неиспользуемый фрагмент памяти; 0 — если используемый блок
   uint32_t size;    // Размер блока, в том числе завершающая запись блока.
} header_t;

typedef struct
{
   uint32_t magic;     // Магическое число, такое же самое, как и в header_t.
   header_t *header; // Указатель на заголовок блока.
} footer_t;

typedef struct
{
   ordered_array_t index;
   uint32_t start_address; // Начало выделяемого пространства памяти.
   uint32_t end_address;   // Конец  выделяемого пространства памяти. Может быть до значения max_address.
   uint32_t max_address;   // Максимальный адрес, до которого куча может расширяться.
   uint8_t supervisor;     // Должны ли дополнительные страницы, запрашиваемые вами, использоваться только в режиме супервизора?
   uint8_t readonly;       // Должны ли дополнительные страницы, запрашиваемые вами, использоваться только в режиме чтения?
} heap_t;

/**
  Создаем новую кучу.
**/
heap_t *create_heap(uint32_t start, uint32_t end, uint32_t max, uint8_t supervisor, uint8_t readonly);
/**
  Allocates a contiguous region of memory 'size' in size. If page_align==1, it creates that block starting
  on a page boundary.
**/
void *alloc(uint32_t size, uint8_t page_align, heap_t *heap);
/**
  Releases a block allocated with 'alloc'.
**/
void free(void *p, heap_t *heap);
#endif // CHAPTER >= 8
