#ifndef PAGING_H
#define PAGING_H

#include "types.h"
#include "idt.h"
#include "heap.h"
#include "stdlib.h"

typedef struct page
{
   uint32_t present    : 1;   // Страница присутствует в памяти
   uint32_t rw         : 1;   // Если сброшен, то страница только для чтения, если установлен, то страница для чтения и записи
   uint32_t user       : 1;   // Если сброшен, то уровень супервизора
   uint32_t accessed   : 1;   // Было ли обращение к странице после последнего ее обновления?
   uint32_t dirty      : 1;   // Выполнялась ли запись на страницу после последнего ее обновления?
   uint32_t unused     : 7;   // Все неиспользуемые и зарезервированные биты
   uint32_t frame      : 20;  // Адрес фрейма (сдвинут вправо на 12 бит)
} page_t;

typedef struct page_table
{
   page_t pages[1024];
} page_table_t;

typedef struct page_directory
{
   /**
      Массив указателей на таблицы страниц.
   **/
   page_table_t *tables[1024];
   /**
      Массив указателей на таблицы страниц, о которых говорилось выше, но указатели указывают *физическое*
      местоположение, используемое при загрузке в регистр CR3.
   **/
   uint32_t tablesPhysical[1024];
   /**
      Физический адрес tablesPhysical. Его потребуется использовать в случае,
      когда мы получаем в ядре память типа куча, а директорий может находиться
      в любом месте виртуальной памяти.
   **/
   uint32_t physicalAddr;
} page_directory_t;

/**
  Настройка среды окружения, директориев страниц и т.д. и 
  включение страничной организации памяти.
**/
void initialise_paging();

/**
  Загружает указанны директорий страниц в регистр CR3.
**/
void switch_page_directory(page_directory_t *new);

/**
  Поиск указателя на необходимую страницу.
  Если make == 1 в таблице страниц, в которой эта страница должна располагаться,
  то страница не создана - создайте страницу!
**/
page_t *get_page(uint32_t address, int make, page_directory_t *dir);

/**
  Обаботчик некорректного обращения к страницам.
**/
void page_fault(registers_t regs); 

#endif
