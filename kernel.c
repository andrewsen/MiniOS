#include "printf.h"
#include "screen.h"
#include "types.h"
#include "keyboard.h"
#include "scanf.h"
#include "string.h"
#include "errno.h"
#include "acpi.h"
#include "io.h"
#include "shutdown.h"
#include "stdlib.h"
#include "gdt.h"
#include "idt.h"
#include "tui.hpp"
#include "multiboot.h"
#include "paging.h"
#ifndef __NO_PAGING
    #include "vmm.h"
    #include "pmm.h"
    #include "heap.h"
#endif
#include "timer.h"
#include "cpu.h"
#include "fs.h"
#include "lab2.h"
//#include "paging.h"
//#include "interrupt.h"

int errno;
//char** split(char * string, int *q, char delim);
//char str_to_byte(char * str);
char * kmem;
//int64_t data;

//DEFINE_ISR(kb, 0) {
//	printf("\nKEYBOARD INTERRUPT\n");
//}

// Сюда поступает вызов из обработчика прерываний, написанного на ассемблере.
void kb_irq(registers_t *reg) {
    //printf("Keyboard interrupt!");
}
void ms_irq(registers_t *reg) {
    printf("Mouse interrupt!");
}
void test_irq(registers_t *reg) {
    printf("Test interrupt!");

}
    
    
void main(struct multiboot *mboot_ptr)
{
    //set_color(Cyan << 4 | White);
    clear_screen();

    set_color(Green);

    if(mboot_ptr->mods_count == 0) panic("Initial RAM Disk wasn't loaded!", 1, -1);
    printf("RD end: 0x%X, RD addr: 0x%X\n", *(uint32_t*)(mboot_ptr->mods_addr+4), *((uint32_t*)mboot_ptr->mods_addr));
    location = *((uint32_t*)mboot_ptr->mods_addr);
    //setKmmBottom((void*)(*(uint32_t*)(mboot_ptr->mods_addr+4) + 1024));
    //uint32_t m1 = 0xDEADC0DE;
    //uint32_t mh = 0x0;
    //uint32_t m2 = 0x1488EBA2;
    //ASMV("int $33");
    printf("Magic: 0x%X\n", *(uint64_t*)(void*)location);
    //uint16_t ** collection = (uint16_t**)DEF_VRAM_BASE;
    //collection[0] = (uint16_t*)DEF_VRAM_BASE;

    //printf("UPPER: 0x%X",mboot_ptr->mem_upper);
    //outb(0x60, 0xF4);

    __enable_fpu();
    init_acpi();



    printf("\nBegin booting... %x\n", mboot_ptr->mem_upper);
    //int i = 100 / 16;
    init_gdt ();
    printf("GDT initialized!\n");
    init_idt ();
    printf("IDT initialized!\n");
    //ASMV("hlt");
    //return;
    register_interrupt_handler (IRQ1, kb_irq);
    register_interrupt_handler (IRQ12, ms_irq);
    register_interrupt_handler (0x3, test_irq);
    register_interrupt_handler (0xd, test_irq);

    printf("Interrupt 0x3:\n");
    //ASMV("int $0x3");
    printf("Interrupt 32:\n");
    ASMV("int $3");



    //return;
#ifndef __NO_PAGING
    init_pmm (0x1000);
    init_vmm ();
    init_heap();
#endif

    //INTS(false);
    //initialise_paging();
    //printf("Reserving 1Kb of RAM...\n");
    kmem = (char*)kmalloc(1024);
    printf("Reserved! 0x%X\n", (int)(int*)kmem);
    initialise_paging();
    int * kmem2 = (int*)kmalloc(sizeof(int) * 32);
    printf("Reserved! 0x%X\n", (int)(void*)kmem2);
    uint16_t* kmem3 = (uint16_t*)kmalloc(sizeof(uint16_t) * 4);
    printf("Reserved! 0x%X\n", (int)(void*)kmem3);
    //printf("Interrupt 14:\n");
    //ASMV("int $14");
    //putchar(200);
    //printf("\n");
    //init_interrupts();
    //set_isr(__get_kb_isr, 34);
    //set_isr(__get_kb_isr, 0x34);
    //INTS(true);

    printf("ACPI inited!\n");

    //for(int i = 0; i < 0xFFFFFFFF; i++) {
    //    if(*(uint32_t*)(void*)(location + i) == 0xEF53FFFF) {
    //        printf("0x%X\n", *(uint8_t*)(void*)i);
    //    }
    //}
    //printf("No magic\n");
    set_color(White);
    printf("Booting complete!\n\t\tSenko Kernel release - 0.0.5-dev\nType 'help' to see command list\n");
    //printf("testing C++:\n");
    //redraw();

    //clear_screen();
    //printf();
    command_routine();

    /*
    init_gdt ();
    init_idt ();
    initialise_paging();
    printf("Paging inited!");
    ASMV("hlt");
    */
}

void command_routine() {
    //char history[255][255];
    int hidx = 0;
    while (true) {
        char ** coms;

        printf("\n>>>");
        char command[255];
        scanf(command);

        //printf("\naddr coms = '%d'\n", (int)coms);
        int q = 0;
        coms = split(command, &q, ' ');
        //printf("\naddr coms2 = '%u'\n", (uint32_t)coms);

        //printf("\ncoms[0] = '%s'\n", coms[0]);
        if(!strcmp(coms[0], "help")) help();
        else if(!strcmp(coms[0], "chcolor")) {
            if(q != 2) {
                printf("\nchcolor: Inv arglist %d", q);
                continue;
            }
            //printf("coms[1]: %s\n", coms[1]);
            set_color_code(str_to_byte(coms[1]));
        }
        else if(!strcmp(coms[0], "clear")) clear_screen();
        else if(!strcmp(coms[0], "reset"))
        {
            //for(u32 i = 0; i < 0xFFFFFF; i++) *(byte*)i = 0xFF;
            printf("\nRESET NOW!");
            wait_s(1);
            return;
        }
        else if(!strcmp(coms[0], "tail")) show_last_rows(0);
        else if(!strcmp(coms[0], "tui")) redraw();
        else if(!strcmp(coms[0], "timer")) wait(1000);
        else if(!strcmp(coms[0], "lab2")) main_lab2();
        else if(!strcmp(coms[0], "beep")) make_sound();
        else if(!strcmp(coms[0], "dofault")) {
            u32 *ptr = 0xABCDEF00;
            u32 pf = *ptr;
        }

        else if(!strcmp(coms[0], "keycode")) { // up - 72, down - 80
            printf("\n");
            u8 key = keyboard_getkey();
            while((char)keycode_to_ascii (key) != 'Q')
            {
                printf("key: %d, char: %c\n", (int)key, keycode_to_ascii (key));
                key = keyboard_getkey();
            }
        }
        else if(!strcmp(coms[0], "memsize")) {
            //ASMV("");
            printf("Temporary unsupported :(");
        }
        else if(!strcmp(coms[0], "file")) {
            char f[255];
            printf("\n");
            scanf(f);
            printf("Trying to read file '%s'", f);
            char buf[10];
            u32 ubuf[10];
            ubuf[0] = ext2_get_file_size(f);
            ext2_read_file(NULL, NULL, f, buf, ubuf, 0);
            printf("\nFILE: \n%s\n", buf);
            printf("File was read correctly (size=%d)", ubuf[0]);
        }
        else if(!strcmp(coms[0], "panic")) {
            int l = 0x3;
            if(q == 2) {
                if(!strcmp(coms[1], "0")) l = 0x0;
                else if(!strcmp(coms[1], "1")) l = 0x1;
                else if(!strcmp(coms[1], "2")) l = 0x2;
                else l = 0x3;
            }
            panic("Test panic!", l, 0xBADC0DE);
        }
        else if(!strcmp(coms[0], "pciscan")) {
            printf("\n");
            PCIScan();
        }
        else if(!strcmp(coms[0], "shutdown")) {
            printf("\nShutdowning...\n");
            shutdown();
        }
        else if(!strcmp(coms[0], "reboot")) reboot();
        else if(!strcmp(coms[0], "about")) {
            printf("\nSenko Operating System - it is simple home OS developed by me\nwithout any codebase!\n");
            printf("Unfortunatly russian is not supported now :(\n%s %s", __DATE__, __TIME__);
        }
        else printf("\nUnknown command: '%s'", coms[0]);

        //kfree(coms);
    }
}

void help() {
    printf("\nCommands:\n");
    printf("  help\t\t-show this message\n");
    printf("  shutdown\t-exit OS\n");
    printf("  tui\t\t-switch to TUI\n");
    printf("  reboot\t-reboot\n");
    printf("  pciscan\t-Scan PCI devices\n");
    printf("  clear\t\t-clear screen\n");
    printf("  about\t\t-about OS\n");
    printf("  panic\t\t-test kernel panic\n");
    printf("  chcolor\t-change text and background color\n");
    printf("  lab2\t\t-run lab 2\n");
    return;
}


void printMBR() {
	clear_screen();
	char* mbr = 0x7C00;
	
	int c = 0;
	bool is55 = false;
	while (true) {
		printf("%c", (int)*mbr);
		if(*mbr == 0x55) is55 = true;
		else if(*mbr == 0xAA && is55) break;
		else is55 = false;
		++mbr;
		++c;
		if(c == 25) {
			keyboard_getchar ();
			c = 0;
		}
	}
    print_p1(">>>");
}

void reboot(void)
{
    uint8_t good = 0x02;
//    u8 b;
    while (good & 0x02)
        good = inb(0x64);
    outb(0x64, 0xFE);
    __asm volatile ("hlt");
}

void make_sound()
{
__asm__("\
    movb $0xB6, %al\n\t\
    outb %al, $0x43\n\t\
    movb $0x0D, %al\n\t\
    outb %al, $0x42\n\t\
    movb $0x11, %al\n\t\
    outb\
    %al, $0x42\n\t\
    inb\
    $0x61, %al\n\t\
    orb\
    $3, %al\n\t\
    outb %al, $0x61\n\t\
");
}

void panic(char * text, int level, int32_t data) {
    ASMV("cli");
    //clear_screen();
    enum colors c = get_color();
    set_color(Red);
    printf("\nKERNEL PANIC:\n");
    printf("REASON:\n");
    printf("  %s\n", text);
    if(data != -1) {
        printf("DATA:\n");
        printf("  0x%X\n", data);
    }
    printf("LEVEL: 0x%X\n", level);
    if(level == 3)  {

        set_color(c);
        ASMV("sti");
        return;
    }
    if(level <= 1) {
        int eax, ebx, ecx, edx, esi, edi, eip, esp, eflags;
        ASMV("mov %%eax, %0" : "=r"(eax));
        ASMV("mov %%ebx, %0" : "=r"(ebx));
        ASMV("mov %%ecx, %0" : "=r"(ecx));
        ASMV("mov %%edx, %0" : "=r"(edx));
        ASMV("mov %%esi, %0" : "=r"(esi));
        ASMV("mov %%edi, %0" : "=r"(edi));
        //ASMV("mov %%eip, %0" : "=r"(eip));
        ASMV("mov %%esp, %0" : "=r"(esp));

        printf("Registers:");
        printf("  EAX=%X EBX=%X ECX=%X EDX=%X\n            ESI=%X EDI=%X ESP=%X\n", eax, ebx, ecx, edx, esi, edi, esp);
        if(level != 0) {
            set_color(c);
            printf(">>>");
        }
    }
    if(level > 1) {
        set_color(c);
        printf(">>>");
        ASMV("sti");
        command_routine();
    }
    if(level == 0) {
        ASMV("sti");
        wait_s(10);
        ASMV("cli");
        printf("\nRebooting!");
        //ASMV("hlt");
        reboot();
    }
    ASMV("sti");

    set_color(c);
    //ASMV("hlt");
}
/*
void count_memory(void)
{
    register uint64_t *mem;
    uint64_t	mem_count, a;
    uint16_t	memkb;
    uint8_t	irq1, irq2;
    uint32_t	cr0;
    uint32_t	mem_end, bse_end;

    /* save IRQ's * /
    irq1=inb(0x21);
    irq2=inb(0xA1);

    /* kill all irq's * /
    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);

    mem_count=0;
    memkb=0;

    // store a copy of CR0
    ASMV("mov %%cr0, %%eax" : "=a"(cr0) :: "eax");

    // invalidate the cache
    // write-back and invalidate the cache
    ASMV("wbinvd");

    // plug cr0 with just PE/CD/NW
    // cache disable(486+), no-writeback(486+), 32bit mode(386+)
    ASMV("mov %%eax, %%cr0" ::
        "a" (cr0 | 0x00000001 | 0x40000000 | 0x20000000) : "eax");

    do {
        memkb++;
        mem_count += 1024*1024;
        mem= (uint64_t*) mem_count;

        a= *mem;
        *mem= 0x55AA55AA;

          // the empty asm calls tell gcc not to rely on what's in its registers
          // as saved variables (this avoids GCC optimisations)
        asm("":::"memory");
        if (*mem!=0x55AA55AA) mem_count=0;
        else {
            *mem=0xAA55AA55;
            asm("":::"memory");
            if(*mem!=0xAA55AA55)
            mem_count=0;
        }

        asm("":::"memory");
        *mem=a;

    } while (memkb<4096 && mem_count!=0);

    ASMV("mov %%eax, %%cr0" :: "a" (cr0) : "eax");

    mem_end = memkb<<20;
    mem = (uint64_t*) 0x413;
    bse_end= (*mem & 0xFFFF) <<6;

    outb(0x21, irq1);
    outb(0xA1, irq2);
}
//DEFINE_ISR(my, 0);
*/























