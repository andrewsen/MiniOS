#ifndef INTRRUPT_H
#define INTRRUPT_H

#include "types.h"

#define INT_VECTORS 35
#define IDT_GATE_INT 0xE
#define INT_KB_VECTOR 0x1
#define IDT_DESC_SIZE 16


#define PIC1 0x20
#define PIC2 0xA0

#define ICW1 0x11
#define ICW4 0x01
#define ALL       0xFF
#define TIMER     0
#define KEYBOARD  1
#define CASCADE   2
#define COM2_4    3
#define COM1_3    4
#define LPT       5
#define FLOPPY    6
#define FREE7     7
#define CLOCK     8
#define FREE9     9
#define FREE10    10
#define FREE11    11
#define PS2MOUSE  12
#define COPROC    13
#define IDE_1     14
#define IDE_2     15
/*
 * PIC macros
 * Master = PIC chip 1
 * Slave = PIC chip 2
 */
#define MASTER          0x20
#define MASTERDATA      0x21
#define SLAVE           0xA0
#define SLAVEDATA       0xA1
#define EOI             0x20


struct int_stack_frame {
  //uint32_t r15d, r14d, r13d, r12d, r11d, r10d, r9d, r8d;
  uint32_t edi, esi, ebp, edx, ecx, ebx, eax;
  uint8_t fxdata[512];
  uint32_t error_code;
  uint32_t eip;
  uint16_t cs;
  uint32_t eflags, esp;
  uint16_t ss;
};

struct idt_desc {
  uint16_t handler0;
  uint16_t cs;
  uint16_t ist : 3;
  uint16_t reserved0 : 5;
  uint16_t type : 4;
  uint16_t zero : 1;
  uint16_t dpl : 2;
  uint16_t present : 1;
  uint16_t handler1;
  uint32_t handler2;
  uint32_t reserved1;
  
	//uint16_t low_offset;                         // low nibble of offset to handler of interrupt 
	//uint16_t selector;                           // GDT selector used 
	//uint16_t settings;                           // settings for int 
	//uint16_t high_offset;                        // high nibble to handler code 
};

struct cpu_table_info {
  uint16_t limit;
  uint32_t base;
} PACKED;


void *get_isr(int vector);
void set_isr(int vector, void *isr);
void create_idt(void);
void load_idt_tr(void);


void INTS(bool on);
#define DEFINE_INT_HANDLER(name)                                        \
  NOINLINE void __handle_##name##_int(                                  \
      UNUSED struct int_stack_frame *stack_frame, UNUSED uint32_t data)
          \
          
#define DEFINE_ISR_WRAPPER(name, handler_name, data)                 \
  NOINLINE void *__get_##name##_isr(void) {                          \
    ASMV("jmp 2f; .align 16; 1: and $(~0xF), %esp");                \
    ASMV("sub $512, %esp; fxsave (%esp)");                          \
    ASMV("push %eax; push %ebx; push %ecx; push %edx; push %ebp");   \
    ASMV("push %esi; push %edi");   \
    ASMV("mov %%esp, %%edi; mov $%P0, %%esi" : : "i"(data));    \
    ASMV("call %P0" : : "i"(__handle_##handler_name##_int));        \
    ASMV("pop %edi; pop %esi");          \
    ASMV("pop %ebp; pop %edx; pop %ecx; pop %ebx; pop %eax");        \
    ASMV("fxrstor (%esp); add $(512 + 8), %esp");                   \
    void *isr;                                                       \
	ASMV("iret; 2: push %eax; mov $1b, %eax"); \
    ASMV ("mov %%eax, %0" : "=m"(isr));      \
    ASMV("pop %eax");   \
    return isr;  \
  }

#define DEFINE_ISR(name, data)             \
  DEFINE_INT_HANDLER(name);                \
  DEFINE_ISR_WRAPPER(name, name, data)     \
  DEFINE_INT_HANDLER(name)
  
#endif
