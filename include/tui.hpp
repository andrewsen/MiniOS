#ifndef TUI_HPP
#define TUI_HPP


#ifndef __cplusplus
#warning __cplusplus not defined!
#endif

#ifdef __cplusplus

#ifndef linux
extern "C" {
	#include "types.h"
	#include "screen.h"
	#include "printf.h"
    #include "string.h"
    #include "stdlib.h"
    #include "assert.h"
    #include "stdlib.h"
    #include "timer.h"
}
#else
#warning linux defined!
    #include <cstdlib>
    #include <cstdio>
    #include <cstring>
    #include <cctype>
    //#includ

    typedef unsigned char uint8_t;
    typedef unsigned short uint16_t;
    typedef unsigned int uint32_t;

    #define MAX_COL 80
    #define MAX_ROW 25
    //#define kmalloc malloc
    #define kfree free

    using namespace std;

    struct vga_cell {
      uint8_t chr;
      uint8_t fcolor : 4;
      uint8_t bcolor : 4;
    };

    extern uint8_t curr_col;
    extern uint8_t curr_row;
    enum colors {Black, Blue, Green, Cyan, Red, Violet, Brown,
                 LightGray, DarkGray, LightBlue, LightGreen, LightCyan, Orange, Pink, Yellow, White};

    vga_cell* getVgaBuffer();
    uint32_t getBufferSize();
    void flushVgaBuffer();
    void clear_screen();
    char *itoa(int n);
#endif

#define ELEMENTS_SIZE 16

struct TPoint {
    uint32_t X;
    uint32_t Y;

    /*TPoint (uint8_t x, uint8_t y) {
        X = x;
        Y = y;
    }*/

    TPoint (uint32_t x, uint32_t y) {
        X = (uint8_t)x;
        Y = (uint8_t)y;
    }
};

/*struct TuiChar {
    struct Border {
        static const uint8_t DUpL = 0;
        static const uint8_t DUpR = 0;
        static const uint8_t DButtomL = 0;
        static const uint8_t DButtomR = 0;

        static const uint8_t SUpL = 0;
        static const uint8_t SUpR = 0;
        static const uint8_t SButtomL = 0;
        static const uint8_t SButtomR = 0;
    };
}; */

class TContainer;

class TElement {
    friend class TContainer;
    friend class TScreen;
protected:
    TContainer * parent;

    static int counter;

    typedef void (*event)(TElement * element);
    event onInitialize = NULL;
    event onDispose = NULL;

    vga_cell * bitmap;
    char name [80];
    TPoint pos = TPoint(0, 0);
    uint8_t heigh;
    uint8_t width;
    uint32_t uid;

    uint8_t borders [6] = {201, 187, 200, 188, 205, 186};
    vga_cell * collection;
    colors ForeColor = White;
    colors BackColor = Black;

    TElement operator=(int i) {
        if(i == NULL) {
            this->uid = NULL;
        }
        return *this;
    }

    void basicInit () {
        //int * v = (int*)DEF_VRAM_BASE;
        //*v = 14534;
        //v//ga_cell ccc;
        //ccc.bcolor = 0;
        int row = 0;
        for (int h = this->pos.Y; h < this->heigh + this->pos.Y; h++) {
            row = this->pos.X + h * MAX_COL;
            for (int w = 0; w < this->width; w++) {
                vga_cell cell;
                cell.bcolor = BackColor;
                cell.fcolor = ForeColor;

                cell.chr = 0;
                if(h == this->pos.Y || h == this->heigh + this->pos.Y -1)
                    cell.chr = borders[4];
                if(w == 0 || w == this->width-1)
                    cell.chr = borders[5];
                collection[row + w] = cell;
            }
        }

        vga_cell cell;
        cell.bcolor = this->BackColor;
        cell.fcolor = this->ForeColor;
        cell.chr = borders[0];
        collection[this->pos.X + this->pos.Y * MAX_COL] = cell;
        cell.chr = borders[1];
        collection[this->pos.X + (this->width -1) + this->pos.Y * MAX_COL] = cell;
        cell.chr = borders[2];
        collection[this->pos.X + (this->pos.Y + this->heigh -1) * MAX_COL] = cell;
        cell.chr = borders[3];
        collection[this->pos.X + (this->width-1) + (this->pos.Y + this->heigh -1) * MAX_COL] = cell;
    }

    void basicDispose() {

    }

    virtual void Initialize () {
        basicInit();
        if(onInitialize != NULL)
            onInitialize(this);
    }

    virtual void Dispose() {
        printf("e->Dispose!");
        /*if(onDispose != NULL)
            onDispose(this);
        basicDispose();*/

    }

public:
    void setName(char* n) {
        strcpy(name, n);
    }

    char* getName() {
        return name;
    }

    uint32_t getUid() {
        return uid;
    }

    void setHeigh(uint8_t h) {
        heigh = h;
    }

    void setWidth(uint8_t w) {
        width = w;
    }

    uint8_t getHeigh() {
        return heigh;
    }

    uint8_t getWidth() {
        return width;
    }

    void setPos(TPoint pos) {
        this->pos = pos;
    }

    TPoint getPos () {
        return pos;
    }



    TElement () {
        uid = NULL;
        pos = TPoint(0, 0);
    }

    colors getForeColor() const;

    void setForeColor(const colors &value);

    colors getBackColor() const;

    void setBackColor(const colors &value);

    event getOnInitialize() const;

    void setOnInitialize(const event &value);

    event getOnDispose() const;

    void setOnDispose(const event &value);
};

class TContainer : public TElement {
protected:

    int index = 0;
    TElement elements[ELEMENTS_SIZE];
    vga_cell bitmap [MAX_ROW * MAX_COL];
public:

    void addElement (TElement * e) {
        //printf("TUI: Adding element %s\n", e->getName());
        e->setPos(TPoint(e->getPos().X + this->pos.X, e->getPos().Y + this->pos.Y));
#ifndef linux
        if(index == ELEMENTS_SIZE-1)
            panic("TUI Element container is full!", 2, -1);
        e->collection = (vga_cell*)(DEF_VRAM_BASE);
#else
        e->collection = getVgaBuffer();
#endif
        //curr_row = 1;
        //printf("Collection: %d, VGA: %X\n", (sizeof(vga_cell)), DEF_VRAM_BASE);
        e->bitmap = (vga_cell*)kmalloc(sizeof(vga_cell) * e->heigh * e->width);

        int row = 0, idx = 0;
        for (int h = e->pos.Y; h < e->heigh + e->pos.Y; h++, idx++) {
            row = e->pos.X + h * MAX_COL;
            for (int w = 0; w < e->width; w++) {
#ifndef linux
                e->bitmap[idx] = ((vga_cell*)(void*)(DEF_VRAM_BASE))[row + w];
#else
                e->bitmap[idx] = getVgaBuffer()[row + w];
#endif
            }
        }

        //for(int i = 0; i < ELEMENTS_SIZE; ++i) {
        //    if(elements[i].uid != NULL) elements[i].Initialize();
        //}

        e->Initialize();
#ifdef linux
        flushVgaBuffer();
        e->uid = index ^ strlen(e->name);
#else
        e->uid = index | strlen(e->name) ^ (int)e;
#endif

        //printf("element %s added!\n", e->name);
        for(int i = 0; i < ELEMENTS_SIZE; i++) {
            if(elements[i].uid == e->uid) {
                //dispose();
                printf("  Dispose reason: Element `%s' already added at %d!\n", e->name, i);
             }
                //panic("TUI: Element already added!", 2);
        }
        elements[index] = *e;
        index++;

        //printf("TUI: element '%s' added! UID = %X\n", e->name, e->uid);
    }

    void removeElement(TElement * e, int pos = -1) {
        bool contains = false;
        if(pos == -1) {
            for(int i = 0; elements[i].uid != NULL; i++)
                if(elements[i].uid == e->uid) {
                    contains = true;
                    pos = i;
                }
            if(!contains) return;

        }
        int row = 0, idx = 0;

        //e->Dispose();
        //printf("TUI: element '%s' ", e->name);
        elements[pos] = NULL;

#ifdef linux
        flushVgaBuffer();
#endif

#ifndef linux
        TElement * temp = (TElement*)kmalloc(sizeof(TElement) * ELEMENTS_SIZE);
#else
        TElement * temp = new TElement [ELEMENTS_SIZE];
#endif
        for(int i = 0; i < ELEMENTS_SIZE; i++)
            temp[i] = elements[i];
        for(int i = 0; i < ELEMENTS_SIZE; i++)
            elements[i].uid = NULL;
        idx = 0;
        for(int i = 0; i < ELEMENTS_SIZE; i++)
            if(temp[i].uid != NULL) {
                elements[idx] = temp[i];
                idx++;
            }

#ifndef linux
        kfree(temp);
#else
        delete [] temp;
#endif
        //printf("removed!\n");
        index--;
        for(int i = 0; i < ELEMENTS_SIZE; ++i) {
            if(elements[i].uid != NULL) elements[i].Initialize();
        }
        return;

    }

    void removeElement(const char * name) {
        for(int i = 0; elements[i].uid != NULL; i++) {
            if(!strcmp(elements[i].name, name)) {
                removeElement(&elements[i], i);
                return;
            }
        }
    }

    void removeElement(uint32_t uid) {
        for(int i = 0; elements[i].uid != NULL; i++) {
            if(elements[i].uid == uid) {
                removeElement(&elements[i], i);
                return;
            }
        }
    }

    void setCell(vga_cell cell, uint32_t X, uint32_t Y)  {
#ifndef linux
        vga_cell * vga = (vga_cell *)DEF_VRAM_BASE;
        vga[X + Y * MAX_COL] = cell;
#else
        vga_cell * vga = getVgaBuffer();
        vga[X + Y * MAX_COL] = cell;
        flushVgaBuffer();
#endif
    }

    vga_cell getCell( uint32_t X, uint32_t Y) {
#ifndef linux
        vga_cell * vga = (vga_cell *)DEF_VRAM_BASE;
#else
        vga_cell * vga = getVgaBuffer();
#endif
        return vga[X + Y * MAX_COL];
    }

    TContainer () {
        for(int i = 0; i < ELEMENTS_SIZE; ++i) {
            elements[i] = TElement();
        }
    }
};

class TScreen : public TContainer {
    TElement elements[ELEMENTS_SIZE];
    vga_cell bitmap [MAX_ROW * MAX_COL];
    uint8_t bcol, brow;

    static TScreen screen;

public:
    //static TScreen screen2;

    static const uint8_t TMAX_ROW = MAX_ROW;
    static const uint8_t TMAX_COL = MAX_COL;

    static const uint8_t FORE_COLOR = White;
    static const uint8_t BACK_COLOR = Black;

    static TScreen * getScreen() {
        return &TScreen::screen;
    }

    void initialize() {
        int idx = 0;
        bcol = curr_col;
        brow = curr_row;
#ifndef linux
        printf("Init TUI 0\n");
        wait_s(2);
        is_cursor_visible(false);
        for(vga_cell* i = (vga_cell*)(void*)DEF_VRAM_BASE; i < (vga_cell*)(void*)(DEF_VRAM_BASE + VRAM_SIZE*2); i++, idx++)
            bitmap[idx] = *i;
        printf("Init TUI 1\n");
        wait_s(2);
#else
        vga_cell * buf = getVgaBuffer();
        for(int i = 0; i < getBufferSize(); i++, idx++)
            bitmap[idx] = buf[i];
#endif
        printf("Init TUI 2\n");
        wait_s(2);
        clear_screen();
        for(int i = 0; i < ELEMENTS_SIZE; i++)
            elements[i] = NULL;
        index = 0;
        printf("Init TUI 3\n");
        wait_s(2);
    }

    void clear() {
        clear_screen();
    }

    void dispose() {
        for(int i = 0; elements[i].uid != NULL; i++) {
            elements[i].Dispose();
            kfree(elements[i].bitmap);
        }

        int idx = 0;
#ifndef linux
        for(vga_cell* i = (vga_cell*)(void*)DEF_VRAM_BASE; i < (vga_cell*)(void*)(DEF_VRAM_BASE + VRAM_SIZE*2); i++, idx++)
            *i = bitmap[idx];
        is_cursor_visible(true);
#else
        vga_cell * buf = getVgaBuffer();
        for(int i = 0; i < getBufferSize(); i++, idx++)
            buf[i] = bitmap[idx];
        flushVgaBuffer();
#endif
        curr_col = bcol;
        curr_row = brow;

        //printf("\nTUI: disposed!\n");
        return;
    }

private:
    TScreen() {

    }
};

class TWindow : public TContainer {
protected:
    char title [80];
    virtual void Initialize() {
        //borders[4] = 179;
        //BackColor = Pink;
        basicInit();
    }

    virtual void Dispose() {
        printf("TWindow dispose!");
    }

public:
    TWindow() {
        printf("Init TUI 4.5\n");
        wait_s(2);
        strcpy(this->name, "window");
    }
};

class TButton : public TElement {
    event click = NULL;
    char text[80];
protected:
    virtual void Initialize() {
        //BackColor = Pink;
        borders[0] = 218;
        borders[1] = 191;
        borders[2] = 192;
        borders[3] = 217;
        borders[4] = 196;
        borders[5] = 179;
        basicInit();
        /*
        for (int h = 0; h < this->heigh; h++)
            for (int w = 0; w < this->width; w++) {
                vga_cell cell;
                cell.bcolor = TScreen::BACK_COLOR;
                cell.fcolor = TScreen::FORE_COLOR;
                if(h == 0 || h == this->heigh-1)
                    cell.chr = 179;
                if(w == 0 || w == this->width-1)
                    cell.chr = 196;
                collection[h][w] = cell;
            }
        vga_cell cell;
        cell.bcolor = TScreen::BACK_COLOR;
        cell.fcolor = TScreen::FORE_COLOR;
        cell.chr = 218;
        collection[0][0] = cell;
        cell.chr = 191;
        collection[0][this->width -1] = cell;
        cell.chr = 192;
        collection[this->heigh -1][0] = cell;
        cell.chr = 217;
        collection[this->heigh -1][this->width -1] = cell;*/
    }


    virtual void Dispose() {
        //printf("TButton dispose!");
    }
public:
    TButton() {
        static int ctr = 1;
        strcpy(this->name, "button");
        strcat(this->name, itoa(ctr));
        ctr += 1;
    }

    TButton(char * name) {
        strcpy(this->name, name);
    }

    void onClick(void (*event)(TElement * element)) {
        this->click = event;
    }
    void Click() {
        if(click != NULL)
            click(this);
        //return click;
    }
};

#endif

#ifdef __cplusplus
extern "C" {
#endif

void redraw();

#ifdef __cplusplus
}
#endif


//#undef kmalloc
//#undef kfree

#endif

/*
    struct vga_cell cell;
    cell.chr = 205;
    cell.bcolor = (uint8_t)Cyan;
    cell.fcolor = (uint8_t)White;
    for (int c = 1; c < MAX_COL; c++) {

        put_cell(cell, 0, c);
    }
    for (int c = 1; c < MAX_COL; c++) {

        put_cell(cell, MAX_ROW-1, c);
    }
    for (int c = 1; c < MAX_COL; c++) {

        put_cell(cell, 2, c);
    }
    cell.chr = 186;
    for (int r = 1; r < MAX_ROW; r++) {

        put_cell(cell, r, 0);
        put_cell(cell, r, MAX_COL-1);
    }
    cell.chr = 201;
    put_cell(cell, 0, 0);
    cell.chr = 187;
    put_cell(cell, 0, MAX_COL-1);
    cell.chr = 200;
    put_cell(cell, MAX_ROW-1, 0);
    cell.chr = 188;
    put_cell(cell, MAX_ROW-1, MAX_COL-1);
    //vvvvvvvv
    printf("                                S e n k o  O S                                ");
    cell.chr = 204;
    put_cell(cell, 2, 0);
    cell.chr = 185;
    put_cell(cell, 2, MAX_COL-1);*/
