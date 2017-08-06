extern "C"
{
    #include "printf.h"
    #include "timer.h"
    #include "keyboard.h"
    #include "heap.h"
    //#include "charmap.h"
}
#include "charmap.h"
#include "gelements.h"

GScreen::GScreen()
{
    children_count = 0;
}

void GScreen::Init(ulong gbase, int height, int width)
{
    //TEST FOR vbe_bytes == 4!!

    this->base = vbe_lfb_addr;
    this->height = height;
    this->width = width;

    children = (GElement**)kmalloc(sizeof(GElement*)*25);
    clear_screen();

    if (!VBE_Setup(width, height))
    {
        printf("Can't init VBE\n");
        return;
    }
    printf("VBE: MODE = 0x%x (%d)\n", base, base);

    if (!VBE_SetMode(vbe_selected_mode | 0x4000))
    {
        printf("Can't set VBE\n");
        return;
    }
}

void GScreen::Add(GElement *element)
{
    if(children_count >= 25)
        return;

    children[children_count++] = element;
    element->Draw(this);
}

void GScreen::Redraw() {
    for(int i = 0; i < children_count; ++i)
    {
        children[i]->Draw(this);
    }
}

void GScreen::DrawText(int x, int y, const char *text)
{
    char ch;
    int char_pos = 0;
    int char_line = 0;

    char *syms = (char*)text;

    while((ch = *syms++) != '\0')
    {
        if(ch == '\n')
        {
            char_pos = 0;
            char_line += CHAR_HEIGHT + 2;
            continue;
        }
        byte* map = charmap[ch - 0x20];
        for(int i = 0; i < CHAR_HEIGHT; ++i)
        {
            for(int j = 0; j < CHAR_WIDTH; ++j)
            {
                if(map[i*CHAR_WIDTH + j])
                {
                    if((x + char_pos + j) >= width)
                        return;
                    SetPixel(x + char_pos + j,
                                     y + char_line + i,
                                     0x0);
                }
                //else
                //    screen->SetPixel(start.x + char_pos + j, start.y + i, 0xFFFFFF);
            }
        }

        char_pos += CHAR_WIDTH + 2;
    }
}

/*void GScreen::DrawText(int x, int y, const char *text)
{
    char ch;
    int char_pos = 0;
    /**(int*)vbe_lfb_addr = 0xFFFFFF;
    *((int*)vbe_lfb_addr+1) = 0xFFFFFF;
    *((int*)vbe_lfb_addr+2) = 0xFFFFFF;* /
    while((ch = *text++) != '\0')
    {
        byte* map;
        //if(ch >= '0' && ch <= '9')
            map = charmap[ch - 0x20];
        //else if(ch >= 'A' && ch <= 'Z')
        //    map = charmap[10 + ch -'A'];
        //else if(ch >= 'a' && ch <= 'z')
        //    map = charmap[10 + ch -'a'];
        //else if(ch == ',')
        //    map = charmap[22 + 'z' - 'a'];
        //else if(ch == ' ')
        //    map = charmap[sizeof(charmap)-1];
        for(int i = 0; i < CHAR_HEIGHT; ++i)
        {
            for(int j = 0; j < CHAR_WIDTH; ++j)
            {
                if(map[i*CHAR_WIDTH + j])
                    SetPixel(x + char_pos + j, y + i, 0x00FF00);
                else
                    SetPixel(x + char_pos + j, y + i, 0);
            }
        }

        char_pos += CHAR_WIDTH + 2;
    }
}*/

void GScreen::SetPixel(int x, int y, int color)
{
    int* base_ptr = (int*)vbe_lfb_addr;
    *(base_ptr + y * width + x) = color & 0xFFFFFF;
}

int GScreen::GetPixel(int x, int y)
{
    return *((int*)vbe_lfb_addr + y * width + x);
}

/*extern "C" void screen_write(int x, int y, const char* text)
{
    GScreen scr;
    printf("GScreen init\n");
    scr.Init(vbe_lfb_addr, 600, 800);
    char ch[2];
    int line = 0, row = 0;
    do
    {
        ch[0] = keyboard_getchar();
        ch[1] = 0;
        if(ch[0] == '\n')
        {
            line = 0;
            row++;
        }
        else if(ch[0] == '\b')
        {
            ch[0] = ' ';
            line--;
            scr.DrawText(x + (CHAR_WIDTH+2)*line, y + (CHAR_HEIGHT+2) * row, ch);
        }
        else
        {
            scr.DrawText(x + (CHAR_WIDTH+2)*line, y + (CHAR_HEIGHT+2) * row, ch);
            line++;
        }
    } while(true);
} */

bool ISelectable::IsSelectable() {
    return selectable;
}

void ISelectable::IsSelectable(bool sel)
{
    selectable = sel;
}
