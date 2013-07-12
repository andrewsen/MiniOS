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
  ���������� � ������� ��������������� ���������/������������� ����� ������
**/
typedef struct
{
   uint32_t magic;   // ���������� �����, ������������ ��� �������� ������ � ��������������.
   uint8_t is_hole;   // 1 � ���� ��� �������������� �������� ������; 0 � ���� ������������ ����
   uint32_t size;    // ������ �����, � ��� ����� ����������� ������ �����.
} header_t;

typedef struct
{
   uint32_t magic;     // ���������� �����, ����� �� �����, ��� � � header_t.
   header_t *header; // ��������� �� ��������� �����.
} footer_t;

typedef struct
{
   ordered_array_t index;
   uint32_t start_address; // ������ ����������� ������������ ������.
   uint32_t end_address;   // �����  ����������� ������������ ������. ����� ���� �� �������� max_address.
   uint32_t max_address;   // ������������ �����, �� �������� ���� ����� �����������.
   uint8_t supervisor;     // ������ �� �������������� ��������, ������������� ����, �������������� ������ � ������ �����������?
   uint8_t readonly;       // ������ �� �������������� ��������, ������������� ����, �������������� ������ � ������ ������?
} heap_t;

/**
  ������� ����� ����.
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
