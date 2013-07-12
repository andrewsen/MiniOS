#include "interrupt.h"
#include "io.h"

static struct idt_desc idt[INT_VECTORS];

//idtr IDTR;
//x86_interrupt IDT[256];// = (x86_interrupt*)0xD00;

void create_idt(void) {
  for (int i = 0; i < INT_VECTORS; i++)
     idt[i] = (struct idt_desc) {
        .cs = CODE_SEGMENT, .ist = 1, .type = IDT_GATE_INT, .present = false
     };
}


void *get_isr(int vector) {
  //struct idt_desc *desc = idt + vector;
  //return (void*)(desc->handler0 + ((uint64_t)desc->handler1 << 16) +
  //               ((uint64_t)desc->handler2 << 32));
  return (void*)NULL;
}

void set_isr(int vector, void *isr) {
  struct idt_desc *desc = idt + vector;
  uint32_t isri = (uint32_t)isr;
  desc->present = isri != 0;
  desc->handler0 = (uint16_t)isri;
  desc->handler1 = (uint16_t)(isri >> 16);
  desc->handler2 = (uint32_t)(isri >> 32);
  /*
	 uint16_t selector = 0x08;
	 uint16_t settings;
     uint32_t offset = (uint32_t)isr;


	 /* get CS selector * /
     asm volatile("movw %%cs,%0" :"=g"(selector));


	 /* set settings options depending on dpl * /
	 //switch(dpl)
	 //{
	 //case 0: settings = INT_0; break;
	 //case 1:
	 //case 2:
	 //case 3: settings = INT_3; break;
	 //}
	 
	 /* set actual values of int * /
	 idt[vector].low_offset   = (offset & 0xFFFF);
	 idt[vector].selector     = selector;
	 idt[vector].settings     = settings;
     idt[vector].high_offset  = (offset >> 16);
     */
}


void load_idt_tr(void) {
  struct cpu_table_info idti = {
    IDT_DESC_SIZE * INT_VECTORS - 1, (uint32_t)idt
  };
  //uint16_t sel = GDT_DESC_SIZE * 3 + GDT_DESC2_SIZE * get_cpu();
  ASMV("lidt %0" : : "m"(idti));
}

/* init_pics()
 * init the PICs and remap them
 */
void init_pics(int pic1, int pic2)
{
	/* send ICW1 */
	outb(PIC1, ICW1);
	outb(PIC2, ICW1);

	/* send ICW2 */
	outb(PIC1 + 1, pic1);	/* remap */
	outb(PIC2 + 1, pic2);	/*  pics */

	/* send ICW3 */
	outb(PIC1 + 1, 4);	/* IRQ2 -> connection to slave */
	outb(PIC2 + 1, 2);

	/* send ICW4 */
	outb(PIC1 + 1, ICW4);
	outb(PIC2 + 1, ICW4);

	/* disable all IRQs */
	outb(PIC1 + 1, 0xFF);
}


/* makes IRQ unavailable so it can't fire an interrupt */
void maskIRQ(byte irq)
{
	if(irq==ALL)
	{
		outb(MASTERDATA,0xFF);
		outb(SLAVEDATA,0xFF);
	}
	else
	{
		irq = irq | (1<<irq);
		if(irq < 8)
			outb(MASTERDATA, irq&0xFF);
		else
			outb(SLAVEDATA, irq>>8);
	}
}

/* opposite of above */
void unmaskIRQ(byte irq)
{
	if(irq==ALL)
	{
		outb(MASTERDATA,0x00);
		outb(SLAVEDATA,0x00);
	}
	else
	{
		irq = irq & (1<<irq);
		if(irq < 8)
			outb(MASTERDATA, irq&0xFF);
		else
			outb(SLAVEDATA, irq>>8);
	}
}

void init_interrupts(void) {
  init_pics(0x20, 0x28);
  //create_gdt();
  create_idt();
  load_idt_tr();
 // LOG_DEBUG("done (CPU: %d)", cpu);
}

void INTS(bool on)
{
	if(on)
	{
		asm("sti");
	}
	else
	{
		asm("cli");
	}
}





























