#include "types.h"
#include "screen.h"
#include "string.h"
#include "io.h"
#include "printf.h"


//#define GREEN    0x3
unsigned char curr_col = 0;
unsigned char curr_row = 0;
static unsigned char color = 0x1;


// Write character at current screen location
#define PUT(c) ( ((unsigned short *) (DEF_VRAM_BASE)) [(curr_row * MAX_COL) + curr_col] = (color << 8) | (c))

struct vga_cell vga_bank[250][MAX_COL];

static unsigned char vga_bank_count = 0;

static inline volatile struct vga_cell *get_cell(int row, int col) {
  return (struct vga_cell*)DEF_VRAM_BASE + row * MAX_COL + col;
}

// Place a character on next screen position
void put_cell(struct vga_cell cell, int row, int col) {
	*get_cell(row, col) = cell;
}

static void cons_putc(int c)
{
    switch (c) 
    {
    case '\t':
        do 
        {
            cons_putc(' ');
        } while ((curr_col % 8) != 0);
        break;
    case '\r':
        curr_col = 0;
        break;
    case '\n':
newline: 
        //curr_col = 0;
        curr_row += 1;
        if (curr_row >= MAX_ROW)
        {
            --curr_row;
            shift_display_rows();
        }
        break;
    case '\b':
        if (curr_col > 0)
        {
            curr_col -= 1;
            PUT(' ');
        }
        else
        {
            curr_col = MAX_COL-1;
            --curr_row;
            PUT(' ');
        }
        break;
    default:
        if (curr_col >= MAX_COL)
        {
            curr_row += 1;
            if (curr_row >= MAX_ROW)
            {
                --curr_row;
                shift_display_rows();
            }
            curr_col = 0;
        }
        PUT(c);
        curr_col += 1;
    };
}

void show_last_rows(unsigned char from) {
from = 0;
  for (int row = 1; row < MAX_ROW; row++) {
    for (int col = 0; col < MAX_COL; col++) {
      *get_cell(row, col) = vga_bank[from][col];
    }
    ++from;
  }
}

void shift_display_rows(void) {
  for (int row = 1; row < MAX_ROW; row++)
    for (int col = 0; col < MAX_COL; col++) {
   	  struct vga_cell c = *get_cell(row, col);
      if(row == 1)
      {
      	vga_bank[vga_bank_count][col] = c;
      }
      *get_cell(row - 1, col) = c;
    }

  for (int col = 0; col < MAX_COL; col++)
    get_cell(MAX_ROW - 1, col)->chr = 0;
    
  ++vga_bank_count;
  if(vga_bank_count == 250) vga_bank_count = 0;
}


void set_vga_cursor(int row, int col) {
  //cursor_row = row, cursor_col = col;
  int off = row * MAX_COL + col;
  outb(0x3D4, 0x0F);
  outb(0x3D5, (unsigned char)(off & 0xFF));
  outb(0x3D4, 0x0E);
  outb(0x3D5, (unsigned char)((off >> 8) & 0xFF));
}

void  is_cursor_visible(bool v)
{
    if(!v) {
        outw(0x3D4,0x200A);

        /* In many sources end line register is reset too. */
        outw(0x3D4,0xB);
        return;
    }
    outw(0x3D4,0xE0A);
    outw(0x3D4,0xF0B);

}

void putchar( int c )
{
    if (c == '\n')
        cons_putc('\r');
    cons_putc(c);
}

void write(const char* str)
{
    while(*str)
        putchar(*(str++));
}
void puts(const char* str)
{
    write(str);
    putchar('\r');
}

void clear_screen( void )
{
    curr_col = 0;
    curr_row = 0;
    
    int i;
    for (i = 0; i < VRAM_SIZE; i++)
        cons_putc(' ');
    
    curr_col = 0;
    curr_row = 0;
    
	//outw(0x3D4, 0xE0A); // make cursor visible
    //outw(0x3D4, 0xF0B);
    
}

void set_color(enum colors col) {
	color = (char)(int)col;
}

void set_color_code(char col) {
	color = col;
}

char get_color_code() {
	return color;
}

enum colors get_color() {
	return color;
}

void print_p1(char * p1) {
	enum colors col = get_color();
	set_color(Blue);
	printf("\n%s", p1);
	set_color(col);
}



char str_to_byte(char * str) {
    if(strlen(str) != 2) {
        printf("\nIllegal hex string: %s\n", str);
        return get_color_code();
    }
    char one = 0, two = 0;
    if('A' <= str[0] && str[0] <= 'F') one = str[0] - 55;
    else if('0' <= str[0] && str[0] <= '9') one = str[0] - 48;
    else printf("\nUnknown character!\n");
    //printf("\nOne: %d", one);

    if('A' <= str[1] && str[1] <= 'F') two = str[1] - 55;
    else if('0' <= str[1] && str[1] <= '9') two = str[1] - 48;
    else printf("\nUnknown character!\n");
    //printf("\nTwo: %d", two);

    one <<= 4;
    printf("\nColor: %X", one | two);
    return one | two;
}

int str_to_int(char * a) {
    int c, sign, offset, n;

    if (a[0] == '-') {  // Handle negative integers
        sign = -1;
    }

    if (sign == -1) {  // Set starting position to convert
        offset = 1;
    }
    else {
        offset = 0;
    }

    n = 0;

    for (c = offset; a[c] != '\0'; c++) {
        n = n * 10 + a[c] - '0';
    }

    if (sign == -1) {
        n = -n;
    }

    return n;
}

long str_to_long(char * a) {
    int c, sign, offset;
    long n;

    if (a[0] == '-') {  // Handle negative integers
        sign = -1;
    }

    if (sign == -1) {  // Set starting position to convert
        offset = 1;
    }
    else {
        offset = 0;
    }

    n = 0;

    for (c = offset; a[c] != '\0'; c++) {
        n = n * 10 + a[c] - '0';
    }

    if (sign == -1) {
        n = -n;
    }

    return n;
}



char get_col() {
	return curr_col;
}
char get_row() {
	return curr_row;
}

int HSVtoRGB(int _h, int _s, int _v)
{
double h = (double)_h / 255.0, s = (double)_s / 255.0, v = (double)_v / 255.0;
double r = 0;
double g = 0;
double b = 0;

if (s == 0)
{
    r = v;
    g = v;
    b = v;
}
else
{
    double varH = h * 6;
    double varI = floor(varH);
    double var1 = v * (1 - s);
    double var2 = v * (1 - (s * (varH - varI)));
    double var3 = v * (1 - (s * (1 - (varH - varI))));

    if (varI == 0)
    {
        r = v;
        g = var3;
        b = var1;
    }
    else if (varI == 1)
    {
        r = var2;
        g = v;
        b = var1;
    }
    else if (varI == 2)
    {
        r = var1;
        g = v;
        b = var3;
    }
    else if (varI == 3)
    {
        r = var1;
        g = var2;
        b = v;
    }
    else if (varI == 4)
    {
        r = var3;
        g = var1;
        b = v;
    }
    else
    {
        r = v;
        g = var1;
        b = var2;
    }
  }
  return ((int)(r * 255) << 16) | ((int)(g * 255) << 8) | (int)(b * 255);
}


void DrawFractal(void)
{
    int x = 0, y = 0, w = 800, h = 600;

    if (!VBE_Setup(w, h))
        return;
    printf("\nVBE: before SetMode()");
    printf("\nVBE: MODE = %x (%d)", vbe_lfb_addr, vbe_lfb_addr);
    //wait_s(5);
    //wait_s(2);
    if (!VBE_SetMode(vbe_selected_mode | 0x4000))
        return;

    double cRe, cIm;
    double newRe, newIm, oldRe, oldIm;
    double zoom = 1, moveX = 0, moveY = 0;
    int color;
    int maxIterations = 300;

    cRe = -0.7;
    cIm = 0.27015;

    for(x = 0; x < w; x++)
    {
        for(y = 0; y < h; y++)
        {
            newRe = 1.5 * (x - w / 2) / (0.5 * zoom * w) + moveX;
            newIm = (y - h / 2) / (0.5 * zoom * h) + moveY;

            int i;
            for(i = 0; i < maxIterations; i++)
            {
                oldRe = newRe;
                oldIm = newIm;
                newRe = oldRe * oldRe - oldIm * oldIm + cRe;
                newIm = 2 * oldRe * oldIm + cIm;
                if((newRe * newRe + newIm * newIm) > 4) break;
            }
            color = HSVtoRGB(i % 256, 255, 255 * (i < maxIterations));

        // Draw pixel
            //wait_s(5);
            *(int *)((char *)vbe_lfb_addr + y * w * vbe_bytes + x * vbe_bytes + 0) = color & 0xFFFFFF;
        }
    }
}







