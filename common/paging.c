
#include "stdlib.h"
#include "paging.h"
#include "string.h"
// Набор bitset для фреймов.
uint32_t *frames;
uint32_t nframes;

extern heap_t *kheap; 

// Определено в kheap.c
extern uint32_t placement_address;

void enable_paging();

page_directory_t* kernel_directory;
page_directory_t* current_directory;

// В алгоритмах для bitset используются макросы.
#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

// Статическая функция для установки бита в наборе bitset для фреймов
static void set_frame(uint32_t frame_addr)
{
   uint32_t frame = frame_addr/0x1000;
   uint32_t idx = INDEX_FROM_BIT(frame);
   uint32_t off = OFFSET_FROM_BIT(frame);
   frames[idx] |= (0x1 << off);
}

// Статическая функция для сброса бита в наборе bitset для фреймов
static void clear_frame(uint32_t frame_addr)
{
   uint32_t frame = frame_addr/0x1000;
   uint32_t idx = INDEX_FROM_BIT(frame);
   uint32_t off = OFFSET_FROM_BIT(frame);
   frames[idx] &= ~(0x1 << off);
}

// Статическая функция для проверки, установлен ли бит
static uint32_t test_frame(uint32_t frame_addr)
{
   uint32_t frame = frame_addr/0x1000;
   uint32_t idx = INDEX_FROM_BIT(frame);
   uint32_t off = OFFSET_FROM_BIT(frame);
   return (frames[idx] & (0x1 << off));
}

// Статическая функция для поиска первого свободного фрейма
static uint32_t first_frame()
{
   uint32_t i, j;
   for (i = 0; i < INDEX_FROM_BIT(nframes); i++)
   {
       if (frames[i] != 0xFFFFFFFF) // нечего не освобождаем, сразу выходим.
       {
           // по меньшей мере, здесь один свободный бит
           for (j = 0; j < 32; j++)
           {
               uint32_t toTest = 0x1 << j;
               if ( !(frames[i]&toTest) )
               {
                   return i*4*8+j;
               }
           }
       }
   }
} 

// Функция выделения фрейма.
void alloc_frame(page_t *page, int is_kernel, int is_writeable)
{
   if (page->frame != 0)
   {
       return; // Фрейм уже выделен, сразу возвращаемся.
   }
   else
   {
       uint32_t idx = first_frame(); // idx теперь является индексом первого свободного фрейма.
       if (idx == (uint32_t)-1)
       {
           // PANIC это всего лишь макрос, которые выдает на экран сообщение, а затем переходит в бесконечный цикл.
           PANIC("No free frames!");
       }
       set_frame(idx*0x1000); // Этот фрейм теперь наш!
       page->present = 1; // Помечаем его как присутствующий.
       page->rw = (is_writeable)?1:0; // Можно ли для страницы выполнять запись?
       page->user = (is_kernel)?0:1; // Находится ли страница в пользовательском режиме?
       page->frame = idx;
   }
}

// Function to deallocate a frame.
void free_frame(page_t *page)
{
   uint32_t frame;
   if (!(frame=page->frame))
   {
       return; // Указанной страницы теперь фактически нет в выделенном фрейме!
   }
   else
   {
       clear_frame(frame); // фрейм теперь снова свободен.
       page->frame = 0x0; // Страницы теперь во фрейме нет.
   }
} 

void initialise_paging()
{
   // Размер физической памяти. Сейчас мы предполагаем,
   // что размер равен 16 MB.
   uint32_t mem_end_page = 0x4000000;

   nframes = mem_end_page / 0x1000;
   frames = (uint32_t*)kmalloc(INDEX_FROM_BIT(nframes));
   memset(frames, 0, INDEX_FROM_BIT(nframes));

   // Давайте создадим директорий страниц.
   kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
   memset(kernel_directory, 0, sizeof(page_directory_t));
   current_directory = kernel_directory;

   // Нам нужна карта идентичности (физический адрес = виртуальный адрес) с адреса
   // 0x0 до конца используемой памяти с тем, чтобы у нас к ним был прозрачный 
   // доступ как если бы страничная организация памяти не использовалась.
   // ЗАМЕТЬТЕ, что мы преднамеренно используем цикл while.
   // Внутри тела цикла мы фактически изменяем адрес placement_address
   // с помощью вызова функции kmalloc(). Цикл while используется здесь, т.к. выход
   // из цикла динамически, а не один раз после запуска цикла.
   int i = 0;
   for (i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += 0x1000)
       get_page(i, 1, kernel_directory); 
       
   // Теперь размещаем страницы, для которых ранее было выполнено отображение.
   for (i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += 0x1000)
       alloc_frame( get_page(i, 1, kernel_directory), 0, 0);

   // Прежде, чем мы включим страничную организацию памяти, нам нужно 
   // зарегистрировать обработчик неверного обращения к страницам.
   register_interrupt_handler(14, page_fault);

   // Теперь включаем страницную организацию памяти!
   switch_page_directory(kernel_directory);

   // Инициализируем кучу ядра .
   kheap = create_heap(KHEAP_START, KHEAP_START+KHEAP_INITIAL_SIZE, 0xCFFFF000, 0, 0); 
}

void switch_page_directory(page_directory_t *dir)
{
   current_directory = dir;
   asm volatile("mov %0, %%cr3":: "r"(&dir->tablesPhysical));
   //uint32_t cr0;
   //asm volatile("mov %%cr0, %0": "=r"(cr0));
   //cr0 |= 0x80000000; // Enable paging!
   //asm volatile("mov %0, %%cr0":: "r"(cr0));
   enable_paging();
}

page_t *get_page(uint32_t address, int make, page_directory_t *dir)
{
   // Помещаем адрес в индекс.
   address /= 0x1000;
   // Находим таблицу страниц, в которой есть этот адрес.
   uint32_t table_idx = address / 1024;
   if (dir->tables[table_idx]) // Если эта таблица уже назначена
   {
       return &dir->tables[table_idx]->pages[address%1024];
   }
   else if(make)
   {
       uint32_t tmp;
       dir->tables[table_idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &tmp);
       memset(dir->tables[table_idx], 0, 0x1000);
       dir->tablesPhysical[table_idx] = tmp | 0x7; // PRESENT, RW, US.
       return &dir->tables[table_idx]->pages[address%1024];
   }
   else
   {
       return 0;
   }
}

void page_fault(registers_t regs)
{
   // Возникло прерывания неверного обращения к странице - page fault.
   // Адрес прерывания запоминается в регистре CR2.
   uint32_t faulting_address;
   asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

   // Код ошибки подробно сообщит нам о том, что случилось.
   int present   = !(regs.err_code & 0x1); // Страница отсутствует
   int rw = regs.err_code & 0x2;           // Операция записи?
   int us = regs.err_code & 0x4;           // Процессор находится в пользовательском режиме?
   int reserved = regs.err_code & 0x8;     // В записи страницы переписаны биты, зарезервированные для нужд процессора?
   int id = regs.err_code & 0x10;          // Причина во время выборки инструкции?

   // Выдача сообщения об ошибке.
   printf("Page fault! ( ");
   if (present) {printf("present ");}
   if (rw) {printf("read-only ");}
   if (us) {printf("user-mode ");}
   if (reserved) {printf("reserved ");}
   printf(") at 0x");
   printf("%x", faulting_address);
   printf("\n");
   PANIC("Page fault");
} 
