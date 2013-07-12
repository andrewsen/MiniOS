#ifndef HEAP_H
#define HEAP_H

#include "types.h"
#include "assert.h"
#include "ordered_array.h"

#define KHEAP_START         0xC0000000
#define KHEAP_INITIAL_SIZE  0x100000
#define HEAP_INDEX_SIZE   0x20000
#define HEAP_MAGIC        0x123890AB
#define HEAP_MIN_SIZE     0x70000

/**
  РРЅС„РѕСЂРјР°С†РёСЏ Рѕ СЂР°Р·РјРµСЂРµ РЅРµРёСЃРїРѕР»СЊР·СѓРµРјРѕРіРѕ С„СЂР°РіРјРµРЅС‚Р°/РёСЃРїРѕР»СЊР·СѓРµРјРѕРіРѕ Р±Р»РѕРєР° РїР°РјСЏС‚Рё
**/
typedef struct
{
   uint32_t magic;   // РњР°РіРёС‡РµСЃРєРѕРµ С‡РёСЃР»Рѕ, РёСЃРїРѕР»СЊР·СѓРµРјРѕРµ РґР»СЏ РєРѕРЅС‚СЂРѕР»СЏ РѕС€РёР±РѕРє Рё РёРЅРґРµРЅС‚РёС„РёРєР°С†РёРё.
   uint8_t is_hole;   // 1 вЂ” РµСЃР»Рё СЌС‚Рѕ РЅРµРёСЃРїРѕР»СЊР·СѓРµРјС‹Р№ С„СЂР°РіРјРµРЅС‚ РїР°РјСЏС‚Рё; 0 вЂ” РµСЃР»Рё РёСЃРїРѕР»СЊР·СѓРµРјС‹Р№ Р±Р»РѕРє
   uint32_t size;    // Р Р°Р·РјРµСЂ Р±Р»РѕРєР°, РІ С‚РѕРј С‡РёСЃР»Рµ Р·Р°РІРµСЂС€Р°СЋС‰Р°СЏ Р·Р°РїРёСЃСЊ Р±Р»РѕРєР°.
} header_t;

typedef struct
{
   uint32_t magic;     // РњР°РіРёС‡РµСЃРєРѕРµ С‡РёСЃР»Рѕ, С‚Р°РєРѕРµ Р¶Рµ СЃР°РјРѕРµ, РєР°Рє Рё РІ header_t.
   header_t *header; // РЈРєР°Р·Р°С‚РµР»СЊ РЅР° Р·Р°РіРѕР»РѕРІРѕРє Р±Р»РѕРєР°.
} footer_t;

typedef struct
{
   ordered_array_t index;
   uint32_t start_address; // РќР°С‡Р°Р»Рѕ РІС‹РґРµР»СЏРµРјРѕРіРѕ РїСЂРѕСЃС‚СЂР°РЅСЃС‚РІР° РїР°РјСЏС‚Рё.
   uint32_t end_address;   // РљРѕРЅРµС†  РІС‹РґРµР»СЏРµРјРѕРіРѕ РїСЂРѕСЃС‚СЂР°РЅСЃС‚РІР° РїР°РјСЏС‚Рё. РњРѕР¶РµС‚ Р±С‹С‚СЊ РґРѕ Р·РЅР°С‡РµРЅРёСЏ max_address.
   uint32_t max_address;   // РњР°РєСЃРёРјР°Р»СЊРЅС‹Р№ Р°РґСЂРµСЃ, РґРѕ РєРѕС‚РѕСЂРѕРіРѕ РєСѓС‡Р° РјРѕР¶РµС‚ СЂР°СЃС€РёСЂСЏС‚СЊСЃСЏ.
   uint8_t supervisor;     // Р”РѕР»Р¶РЅС‹ Р»Рё РґРѕРїРѕР»РЅРёС‚РµР»СЊРЅС‹Рµ СЃС‚СЂР°РЅРёС†С‹, Р·Р°РїСЂР°С€РёРІР°РµРјС‹Рµ РІР°РјРё, РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊСЃСЏ С‚РѕР»СЊРєРѕ РІ СЂРµР¶РёРјРµ СЃСѓРїРµСЂРІРёР·РѕСЂР°?
   uint8_t readonly;       // Р”РѕР»Р¶РЅС‹ Р»Рё РґРѕРїРѕР»РЅРёС‚РµР»СЊРЅС‹Рµ СЃС‚СЂР°РЅРёС†С‹, Р·Р°РїСЂР°С€РёРІР°РµРјС‹Рµ РІР°РјРё, РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊСЃСЏ С‚РѕР»СЊРєРѕ РІ СЂРµР¶РёРјРµ С‡С‚РµРЅРёСЏ?
} heap_t;

/**
  РЎРѕР·РґР°РµРј РЅРѕРІСѓСЋ РєСѓС‡Сѓ.
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

#endif
