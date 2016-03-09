#ifndef ASM_H
#define ASM_H

#include "types.h"

static inline void
asm_inb (u32 port, u8 *data)
{
	__asm volatile ("inb %%dx,%%al"
		      : "=a" (*data)
		      : "d" (port));
}

static inline void
asm_inw (u32 port, u16 *data)
{
	__asm volatile ("inw %%dx,%%ax"
		      : "=a" (*data)
		      : "d" (port));
}

static inline void
asm_inl (u32 port, u32 *data)
{
	__asm volatile ("inl %%dx,%%eax"
		      : "=a" (*data)
		      : "d" (port));
}

static inline void
asm_outb (u32 port, u8 data)
{
	__asm volatile ("outb %%al,%%dx"
		      :
		      : "a" (data)
		      , "d" (port));
}

static inline void
asm_outw (u32 port, u16 data)
{
	__asm volatile ("outw %%ax,%%dx"
		      :
		      : "a" (data)
		      , "d" (port));
}

static inline void
asm_outl (u32 port, u32 data)
{
	__asm volatile ("outl %%eax,%%dx"
		      :
		      : "a" (data)
		      , "d" (port));
}
#endif
