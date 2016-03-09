// timer.h -- Определения интерфейса для всех функций, относящихся к PIT.
// Написано для руководств по разработке ядра - автор James Molloy

#ifndef TIMER_H
#define TIMER_H

#include "types.h"

void init_timer(double frequency);
void set_timer_handler(void (*handler)());

void wait(int ms);
void wait_s(int seconds);

#endif
