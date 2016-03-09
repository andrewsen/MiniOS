// timer.c -- �������������� PIT � ������������ ���������� �������� �������.
// �������� ��� ���������� �� ���������� ���� - ����� James Molloy

#include "timer.h"
#include "idt.h"
#include "printf.h"
#include "io.h"

uint32_t tick = 0;

static void timer_callback(registers_t regs)
{
   //printf("Tick: %d\n", tick);
   ++tick;
   //monitor_write_dec(tick);
   //monitor_write("\n");
}

void init_timer(double frequency)
{
   // ������ ����� ������������ �������� ����� (callback) ������ �������.
   register_interrupt_handler(IRQ0, &timer_callback);

   // ��������, ������� �� �������� � PIT, �������� ���������, �� ������� ����� �������� �������� �������
   // (1193180 Hz) ��� ����, ����� �������� ����������� ��� �������. ����� ��������, ��� �������� ������
   // ���� ���������� ��������� � ���, ����� ���������� � 16 ����� ��������� �����.
   uint32_t divisor = (uint32_t)(1193180 / frequency);

   //divisor = 119318;

   // �������� ���� �������.
   outb(0x43, 0x36);

   // �������� ������ ���������� ��������, ��� ��� �� ����� ��� �� ������� � ������� �����.
   uint8_t l = (uint8_t)(divisor & 0xFF);
   uint8_t h = (uint8_t)( (divisor>>8) & 0xFF );

   // �������� �������� �������.
   outb(0x40, l);
   outb(0x40, h);
   //?
   //printf("\n?\n");
}



//void init_timer(uint32_t frequency);
void set_timer_handler(void (*handler)()) {

}

void wait(int ms) {
    tick = 0;
    //printf("\nBEGIN\n");
    init_timer(100);
    while(tick * 10 <= ms) ;
    //outb(0x0, 0x0);
    //outb(0x0, 0x0);
    //printf("\nWAITED!\n");
    //
    tick = 0;
}

void wait_s(int seconds) {
    tick = 0;
    //printf("\nBEGIN\n");
    init_timer(100);
    while(tick <= seconds * 100) ;
    //outb(0x0, 0x0);
    //outb(0x0, 0x0);
    //printf("\nWAITED!\n");
    //
    tick = 0;
}
