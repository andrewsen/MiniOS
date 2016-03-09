// ordered_array.h – Интерфейс создания упорядоченного массива и 
// добавления к нему или удаления из него элементов.
// Написано для  руководств по разработке ядра - автор James Molloy

#ifndef ORDERED_ARRAY_H
#define ORDERED_ARRAY_H

#include "types.h"
#include "assert.h"

/**
Сортировка этого массива происходит при вставке элементов — он всегда будет отсортирован 
(между двумя последовательными к нему обращениями)
В нем можно хранить все, что угодно; тип хранящихся объектов определяется как void*,
поэтому можно хранить uint32_t или какой-нибудь другой указатель .
**/
typedef void* type_t;
/**
 Предикат должен возвращать ненулевое значение в случае, если первый аргумент меньше, 
чем второй. В противном случае должно быть возвращено нулевое значение. 
**/
typedef int8_t (*lessthan_predicate_t)(type_t,type_t);
typedef struct
{
   type_t *array;
   uint32_t size;
   uint32_t max_size;
   lessthan_predicate_t less_than;
} ordered_array_t;

/**
  Стандартный предикат less than (меньше, чем)
**/
int8_t standard_lessthan_predicate(type_t a, type_t b);

/**
 Создание упорядоченного массива
**/
ordered_array_t create_ordered_array(uint32_t max_size, lessthan_predicate_t less_than);
ordered_array_t place_ordered_array(void *addr, uint32_t max_size, lessthan_predicate_t less_than);

/**
  Уничтожение упорядоченного массива
**/
void destroy_ordered_array(ordered_array_t *array);

/**
  Добавление элемента в массив
**/
void insert_ordered_array(type_t item, ordered_array_t *array);

/**
Поиск элемента по индексу i.
**/
type_t lookup_ordered_array(uint32_t i, ordered_array_t *array);

/**
  Удаление из массива элемента, расположенного по индексу i
**/
void remove_ordered_array(uint32_t i, ordered_array_t *array);

#endif // ORDERED_ARRAY_H
