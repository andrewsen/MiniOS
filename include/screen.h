#ifndef _SCREEN_H
#define _SCREEN_H

#include "types.h"

#define MAX_COL  80		// Maximum number of columns 
#define MAX_ROW  25		// Maximum number of rows 
#define VRAM_SIZE (MAX_COL*MAX_ROW)	// Size of screen, in short's 
#define DEF_VRAM_BASE 0xb8000	// Default base for video memory


extern unsigned char curr_col;
extern unsigned char curr_row;

enum colors {Black, Blue, Green, Cyan, Red, Violet, Brown, LightGray, DarkGray, LightBlue, LightGreen, LightCyan, Orange, Pink, Yellow, White};
struct vga_cell {
  uint8_t chr;
  uint8_t fcolor : 4;
  uint8_t bcolor : 4;
}; 

void clear_screen( void );
void putchar( int c );
void set_color(enum colors col);
enum colors get_color();
void set_color_code(char col);
void is_cursor_visible(bool v);

char get_color_code();
void print_p1(char * p1);
char get_col();
char get_row();


void put_cell(struct vga_cell cell, int row, int col);
char str_to_byte(char * str);
void show_last_rows(unsigned char from);

#endif
