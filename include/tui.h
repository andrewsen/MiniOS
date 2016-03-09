
#ifndef TUI_H
#define TUI_H

#include "types.h"

typedef struct {
	void (*on_click)(void * object);
} click_handler_t;

typedef struct {
	click_handler_t button_click;
	char text [255];
	uint8_t heigh;
	uint8_t width;
	uint8_t posX;
	uint8_t posY;
} button_t;

#endif
