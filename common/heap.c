//#if CHAPTER >= 8
// 
// heap.c -- Defines functions and structures for the kernel heap.
//           Written for JamesM's kernel development tutorials.
//

#include "heap.h"
#include "stdlib.h"
#include "paging.h"
#include "assert.h"
#include "timer.h"

extern page_directory_t* kernel_directory;
extern page_directory_t* current_directory;
heap_t *kheap;

int i = 0;

static int find_smallest_hole(uint32_t size, uint8_t page_align, heap_t *heap)
{
   // Находим наименьший свободный фрагмент, который подходит.
   uint32_t iterator = 0;
   while (iterator < heap->index.size)
   {
       header_t *header = (header_t *)lookup_ordered_array(iterator, &heap->index);
       // Если пользователь запросил память, которая выровнена по границе
       if (page_align > 0)
       {
           // Выравниваем по границе начало заголовка.
           uint32_t location = (uint32_t)header;
           uint32_t offset = 0;
           if ((location+sizeof(header_t)) & 0xFFFFF000 != 0)
               offset = 0x1000 /* размер страницы */  - (location+sizeof(header_t))%0x1000;
           uint32_t hole_size = (uint32_t)header->size - offset;
           printf("Hole size: %d, size: %d\n", hole_size, size);
           // Теперь подходит?
           if (hole_size >= (uint32_t)size)
               break;
       }
       else if (header->size >= size)
           break;
       iterator++;
   }
   // Когда выходить из цикла?
   if (iterator == heap->index.size)
       return -1; // Мы дошли до конца и ничего не нашли.
   else
       return iterator;
}
static char header_t_less_than(void*a, void *b)
{
   return (((header_t*)a)->size < ((header_t*)b)->size)?1:0;
}

heap_t *create_heap(uint32_t start, uint32_t end_addr, uint32_t max, uint8_t supervisor, uint8_t readonly)
{
   heap_t *heap = (heap_t*)kmalloc(sizeof(heap_t));

   // Мы предполагаем, что startAddress и endAddress выровнены по границе страниц.
   ASSERT(start%0x1000 == 0);
   ASSERT(end_addr%0x1000 == 0);

   // Инициализируем список индексов.
   heap->index = place_ordered_array( (void*)start, HEAP_INDEX_SIZE, &header_t_less_than);

   // Сдвигаем начальный адрес вперед, куда мы можем начать помещать данные.
   start += sizeof(type_t)*HEAP_INDEX_SIZE;

   // Обесчьте, чтобы начальный адрес был выровнен по границе страниц.
   if (start & 0xFFFFF000 != 0)
   {
       start &= 0xFFFFF000;
       start += 0x1000;
   }
   // Запишите начальный, конечный и максимальный адреса в структуру памяти типа куча.
   heap->start_address = start;
   heap->end_address = end_addr;
   heap->max_address = max;
   heap->supervisor = supervisor;
   heap->readonly = readonly;

   // Мы начинаем с одного большого фрагмента свободной памяти, указанной в списке индексов.
   header_t *hole = (header_t *)start;
   hole->size = end_addr-start;
   hole->magic = HEAP_MAGIC;
   hole->is_hole = 1;
   insert_ordered_array((void*)hole, &heap->index);

   return heap;
}

static void expand(uint32_t new_size, heap_t *heap)
{
   // Проверяем правильность значений.
   ASSERT(new_size > heap->end_address - heap->start_address);
   // Получаем следующую ближайшую границу страницы.
   if (new_size&0xFFFFF000 != 0)
   {
       new_size &= 0xFFFFF000;
       new_size += 0x1000;
   }
   // Убеждаемся, что мы не перехитрили сами себя
   ASSERT(heap->start_address+new_size <= heap->max_address);

   // Это значение должно всегда быть на границе страницы.
   uint32_t old_size = heap->end_address-heap->start_address;
   uint32_t i = old_size;
   while (i < new_size)
   {
       alloc_frame( get_page(heap->start_address+i, 1, kernel_directory),
                    (heap->supervisor)?1:0, (heap->readonly)?0:1);
       i += 0x1000 /* page size */;
   }
   heap->end_address = heap->start_address+new_size;
}

static uint32_t contract(uint32_t new_size, heap_t *heap)
{
   // Проверяем правильность значений.
   ASSERT(new_size < heap->end_address-heap->start_address);
   // Получаем следующую ближайшую границу страницы.
   if (new_size&0x1000)
   {
       new_size &= 0x1000;
       new_size += 0x1000;
   }
   // Не следует уменьшать размер слишком сильно!
   if (new_size < HEAP_MIN_SIZE)
       new_size = HEAP_MIN_SIZE;
   uint32_t old_size = heap->end_address-heap->start_address;
   uint32_t i = old_size - 0x1000;
   while (new_size < i)
   {
       free_frame(get_page(heap->start_address+i, 0, kernel_directory));
       i -= 0x1000;
   }
   heap->end_address = heap->start_address + new_size;
   return new_size;
}

void *alloc(uint32_t size, uint8_t page_align, heap_t *heap)
{

   // Проверяем, что мы также учитываем размер заголовка / завершающей записи блока.
    uint32_t new_size = size + sizeof(header_t) + sizeof(footer_t);
    // Находим наименьший фрагмент свободный памяти, которые соответствует нашему запросу.
    int iterator = find_smallest_hole(new_size, page_align, heap);

    if (iterator == -1) // Если мы не нашли подходящего фрагмента
    {
       // Сохраняем некоторые ранее полученные данные.
       uint32_t old_length = heap->end_address - heap->start_address;
       uint32_t old_end_address = heap->end_address;

       // Нам нужно выделить немного больше памяти.
       expand(old_length+new_size, heap);
       uint32_t new_length = heap->end_address - heap->start_address;

       //printf("Old size: %d, New size: %d, i: %d\n", size, new_size, i);
       ++i;
       //wait_s(1);
       // Находим самый последний заголовок. (Последний не по размеру, а по месторасположению).
       iterator = 0;
       // В переменных хранятся индекс и значение самого последнего заголовка, найденного к текущему моменту.
       uint32_t idx = -1; uint32_t value = 0x0;
       while (iterator < heap->index.size)
       {
           uint32_t tmp = (uint32_t)lookup_ordered_array(iterator, &heap->index);
           if (tmp > value)
           {
               value = tmp;
               idx = iterator;
           }
           iterator++;
       }

       // Если мы вообще не нашли НИКАКИХ заголовков, на нужно один заголовок добавить.
       if (idx == -1)
       {
           header_t *header = (header_t *)old_end_address;
           header->magic = HEAP_MAGIC;
           header->size = new_length - old_length;
           header->is_hole = 1;
           footer_t *footer = (footer_t *) (old_end_address + header->size - sizeof(footer_t));
           footer->magic = HEAP_MAGIC;
           footer->header = header;
           insert_ordered_array((void*)header, &heap->index);
       }
       else
       {
           // Последний заголовок нужно настроить.
           header_t *header = lookup_ordered_array(idx, &heap->index);
           header->size += new_length - old_length;
           // Переписываем завершающую запись.
           footer_t *footer = (footer_t *) ( (uint32_t)header + header->size - sizeof(footer_t) );
           footer->header = header;
           footer->magic = HEAP_MAGIC;
       }
       // Теперь у над достаточно места. Снова обращаемся к этой функции рекурсивно.
       return alloc(size, page_align, heap);
    }

    //Здесь мы настраиваем размер запрашиваемого блока с учетом его заголовка и его завершающей записи.
    //Затем мы с помощью нашей функции find_smallest_hole запрашиваем наименьший свободный фрагмент, который подходит.
    //Если мы не смогли найти такой фрагмент (find_smallest_hole () == -1), мы переходим к коду обработки некоторой ошибки.
    //Такое решение в некоторой степени спорно, поэтому я вернусь к нему, чтобы его пояснить.

    header_t *orig_hole_header = (header_t *)lookup_ordered_array(iterator, &heap->index);
    uint32_t orig_hole_pos = (uint32_t)orig_hole_header;
    uint32_t orig_hole_size = orig_hole_header->size;
    // Сюда мы попадаем, если мы должны разделить фрагмент, который мы нашли, на две части.
    // Является ли размер исходного (запрашиваемого) фрагмента меньше накладных расходов на добавление нового фрагмента?
    if (orig_hole_size-new_size < sizeof(header_t)+sizeof(footer_t))
    {
      // Тогда просто увеличиваем запрашиваемый размер до размера найденного фрагмента.
      size += orig_hole_size-new_size;
      new_size = orig_hole_size;
    }
    //Здесь мы из индексного списка, выданного нам функцией find_smallest_hole, получаем указатель на заголовок.
    //Затем мы сохраняем адрес и размер этого заголовка на случай, если позже нам потребуется его переписать.
    //После этого мы решаем, стоит ли делить фрагмент на два фрагмента (то есть, будет ли достаточно во втором фрагменте оставшегося места?)
    //Если нет, то мы увеличиваем запрашиваемый размер так, чтобы не делать разбиение фрагмента.

    // Если нам нужно выравнивать данные по границе страницы, то выполните это сейчас
    // и сделайте так, чтобы новый фрагмент находился перед нашим блоком памяти.
    if (page_align && orig_hole_pos&0xFFFFF000)
    {
        uint32_t new_location   = orig_hole_pos + 0x1000 /* page size */ - (orig_hole_pos&0xFFF) - sizeof(header_t);
        header_t *hole_header = (header_t *)orig_hole_pos;
        hole_header->size     = 0x1000 /* page size */ - (orig_hole_pos&0xFFF) - sizeof(header_t);
        hole_header->magic    = HEAP_MAGIC;
        hole_header->is_hole  = 1;
        footer_t *hole_footer = (footer_t *) ( (uint32_t)new_location - sizeof(footer_t) );
        hole_footer->magic    = HEAP_MAGIC;
        hole_footer->header   = hole_header;
        orig_hole_pos         = new_location;
        orig_hole_size        = orig_hole_size - hole_header->size;
    }
    else
    {
      // В противном случае этот фрагмент нам больше не нужен, удалите его из списка индексов.
      remove_ordered_array(iterator, &heap->index);
    }
    //Если пользователь хочет, чтобы его память была выровнена по границе страниц, мы это здесь делаем.
    //Новое положение размещаемого заголовка рассчитывается путем перехода к следующей границе страницы,
    //а затем вычитания размера заголовка. Затем заполняются атрибуты заголовка нового фрагмента и добавляется завершающая запись.
    //Обратите внимание, что поскольку мы создаем новый фрагмент по адресу старого фрагмента, мы, по сути,
    //повторно используем старый фрагмент, так что удалять его из списка индексов фрагментов не требуется.

    // Переписываем исходный заголовок ...
    header_t *block_header  = (header_t *)orig_hole_pos;
    block_header->magic     = HEAP_MAGIC;
    block_header->is_hole   = 0;
    block_header->size      = new_size;
    // ... И завершающую запись
    footer_t *block_footer  = (footer_t *) (orig_hole_pos + sizeof(header_t) + size);
    block_footer->magic     = HEAP_MAGIC;
    block_footer->header    = block_header;
    //Здесь все должно быть понятно — все, что мы делаем, это корректируем заголовок и завершающую запись, в том числе и магические числа.

    // Нам  после выделенного блока может потребоваться записать новый фрагмент.
    // Мы делаем это только если размер нового фрагмента положительный ...
    if (orig_hole_size - new_size > 0)
    {
        header_t *hole_header = (header_t *) (orig_hole_pos + sizeof(header_t) + size + sizeof(footer_t));
        hole_header->magic    = HEAP_MAGIC;
        hole_header->is_hole  = 1;
        hole_header->size     = orig_hole_size - new_size;
        footer_t *hole_footer = (footer_t *) ( (uint32_t)hole_header + orig_hole_size - new_size - sizeof(footer_t) );
        if ((uint32_t)hole_footer < heap->end_address)
        {
            hole_footer->magic = HEAP_MAGIC;
            hole_footer->header = hole_header;
        }
        // Помещаем новый фрагмент в индексный список;
        insert_ordered_array((void*)hole_header, &heap->index);
    }
    //Если нам требуется разделить наш фрагмент на два фрагмента, мы делаем это здесь, создавая при этом новый фрагмент.

    // ...И мы все сделали!
    return (void *) ( (uint32_t)block_header+sizeof(header_t) );
}

void free(void *p, heap_t *heap)
{
    // Выход в случае нулевых указателей.
    if (p == 0)
       return;

    // Берем заголовок и завершающую запись, связанные с этим указателем.
    header_t *header = (header_t*) ( (uint32_t)p - sizeof(header_t) );
    footer_t *footer = (footer_t*) ( (uint32_t)header + header->size - sizeof(footer_t) );

    // Проверяем правильность значений.
    ASSERT(header->magic == HEAP_MAGIC);
    ASSERT(footer->magic == HEAP_MAGIC);
    //Сначала мы находим заголовок путем вычитания sizeof(header_t) из p, а затем используем его для того, чтобы найти завершающую запись. Всегда хорошо проверить правильность значений, поскольку можно сразу обнаружить ситуацию, когда в вашем коде перезаписаны важные данные.

    // Создаем фрагмент свободной памяти.
    header->is_hole = 1;

    // Хотим ли мы добавит этот заголовок в список индексов 'фрагменты свободной памяти' index?
    char do_add = 1;
    //Этот блок в настоящее время освобожден, поэтому сейчас он является фрагментом свободной памяти. Мы также создаем переменную, в которой указываем, должны ли мы добавить заголовок в список индексов (смотрите описание алгоритма).

    // Унификация слева
    // Если непосредственно слева от фрагмента находится завершающая запись другого фрагмента ...
    footer_t *test_footer = (footer_t*) ( (uint32_t)header - sizeof(footer_t) );
    if (test_footer->magic == HEAP_MAGIC &&
       test_footer->header->is_hole == 1)
    {
       uint32_t cache_size = header->size; // Кэшируем наш текущий размер.
       header = test_footer->header;     // Перезаписываем вместо нашего новый заголовок.
       footer->header = header;          // Заменяем нашу завершающую запись новой, на которую указывает новый заголовок.
       header->size += cache_size;       // Изменяем размер.
       do_add = 0;                       // Поскольку этот заголовок уже в индексном списке, нам не нужно добавлять его снова.
    }
    //В этой части кода выполняется наша левая унификация (объединение влево). Если мы вычтем sizeof(header_t) из адреса заголовка, мы можем получить указатель на завершающую запись. Мы проверяем, что это действительно правильная завершающая запись при помощи проверки в ней магического числа и проверкой того, что это фрагмент (а не выделенная память!). Если это так, мы перепишем нашу завершающую запись, на которую будет указывать переменная test_footer, находящаяся в заголовке, изменяем наш размер и указываем в алгоритме не добавлять запись в список индексов фрагментов (поскольку заголовок только что унифицированного фрагмента уже находится в индексном списке!).

    // Унификация справа
    // Если непосредственно справа от фрагмента находится заголовок другого фрагмента ...
    header_t *test_header = (header_t*) ( (uint32_t)footer + sizeof(footer_t) );
    if (test_header->magic == HEAP_MAGIC &&
       test_header->is_hole)
    {
       header->size += test_header->size; // Увеличиваем наш размер.
       test_footer = (footer_t*) ( (uint32_t)test_header + // Переписываем завершающую запись, на которую указывает наш заголовок.
                                   test_header->size - sizeof(footer_t) );
       footer = test_footer;
       // Находим и удаляем этот заголовок из списка индексов.
       uint32_t iterator = 0;
       while ( (iterator < heap->index.size) &&
               (lookup_ordered_array(iterator, &heap->index) != (void*)test_header) )
           iterator++;

       // Убеждаемся, что мы действительно нашли правильный объект.
       ASSERT(iterator < heap->index.size);
       // Удаляем его.
       remove_ordered_array(iterator, &heap->index);
    }
    //Аналогичным образом в этом коде выполняется правая унификация. Опять же, мы проверяем, находится сразу справа заголовок и является ли пространство памяти справа фрагментом неиспользуемой памяти. Перепишем его завершающую запись так, чтобы она указывала на наш заголовок, а затем удалим заголовок фрагмента, находящегося справа, из списка индексов.

    // Если положение завершающей записи совпадает с конечным адресом кучи, мы можем уменьшить кучу.
    if ( (uint32_t)footer+sizeof(footer_t) == heap->end_address)
    {
       uint32_t old_length = heap->end_address-heap->start_address;
       uint32_t new_length = contract( (uint32_t)header - heap->start_address, heap);
       //Проверяем насколько большой будет куча после изменения размера.
       if (header->size - (old_length-new_length) > 0)
       {
           // Мы еще существуем - Теперь собственно изменяем размер.
           header->size -= old_length-new_length;
           footer = (footer_t*) ( (uint32_t)header + header->size - sizeof(footer_t) );
           footer->magic = HEAP_MAGIC;
           footer->header = header;
       }
       else
       {
           // Теперь нас нет  :(. Удаляем нас из списка индексов.
           uint32_t iterator = 0;
           while ( (iterator < heap->index.size) &&
                   (lookup_ordered_array(iterator, &heap->index) != (void*)test_header) )
               iterator++;
           // Если мы себя не нашли, то нам и нечего удалять.
           if (iterator < heap->index.size)
               remove_ordered_array(iterator, &heap->index);
       }
    }
    if (do_add == 1)
      insert_ordered_array((void*) header, &heap->index);
}











