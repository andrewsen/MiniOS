#ifndef _CPU_H
#define _CPU_H

#include "types.h"

// Объявление нескольких битов в CR0 которые потребуются для инициализации FPU
#define X86_CR0_EM              0x00000004 /* Require FPU Emulation    (RO) */
#define X86_CR0_TS              0x00000008 /* Task Switched            (RW) */


static inline unsigned int __get_cr0( void )
{
    register uint32_t res;
    __asm__ __volatile__("mov %%cr0, %0\n\t" : "=r" (res) : );
    return(res);
}

static inline void __set_cr0( uint32_t addr )
{
    __asm__ __volatile__("mov %0, %%cr0\n\t" : : "r" (addr));
}

// (11) Функция, которая включает поддержку FPU на ядре процессора
static inline void __enable_fpu()
{
  uint32_t cr0 = __get_cr0();
  cr0 &= ~(X86_CR0_TS | X86_CR0_EM);
  __set_cr0(cr0);
  asm ( "fnclex\r\nfninit\r\n"   : : );
}

#endif
