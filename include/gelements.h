#ifndef GELEMENTS_H
#define GELEMENTS_H

#include "gui.h"
#include <runtime.h>
#include <vector>

extern "C"
{
    #include "keyboard.h"
    #include "string.h"
    #include "timer.h"
}

struct Point
{
    int x, y;
};

class GPanel : public GElement
{
protected:
    Point start;
    int height = 0, width = 0;
    int color = 0;
    int back_color = -1;
    int border_color = 0;
    int border_width = 0;
public:
    GPanel()
    {

    }

    virtual void Draw(GScreen *screen)
    {
        if(back_color != -1)
        for(int i = 0; i < height-border_width*2; ++i)
            for(int j = 0; j < width-border_width*2; ++j)
            {
                screen->SetPixel(border_width+start.x+j, border_width+start.y+i, back_color);
            }
        for(int i = 0; i < height; ++i)
            for(int j = 0; j < border_width; ++j)
            {
                screen->SetPixel(start.x+j, start.y+i, border_color);
                screen->SetPixel(start.x+width-border_width+j, start.y+i, border_color);
            }
        for(int i = border_width; i < width-border_width; ++i)
            for(int j = 0; j < border_width; ++j)
            {
                screen->SetPixel(start.x+i, start.y+j, border_color);
                screen->SetPixel(start.x+i, start.y+height-border_width+j, border_color);
            }
    }

    int GetBorderColor() { return border_color; }
    int GetBorderWidth() { return border_width; }

    void SetBorderColor(int color) { border_color = color; }
    void SetBorderWidth(int width) { border_width = width; }

    int GetBackgroundColor() { return back_color; }
    void SetBackgroundColor(int color) { back_color = color; }

    int GetHeight() { return height; }
    int GetWidth()  { return width; }

    void SetHeight(int height)  { this->height = height; }
    void SetWidth(int width)  { this->width = width; }

    void SetX(int x)  { start.x = x; }
    void SetY(int y)  { start.y = y; }

    Point GetLocation() { return start; }

    void SetLocation(const Point &p) { start = p; }
    void SetLocation(int x, int y)
    {
        start.x = x;
        start.y = y;
    }
};

class GContainer : public GPanel
{
protected:
    vector<GElement*> elements;
public:
    virtual void AddElement(GElement* elem);

    virtual void RemoveElement(GElement* elem);

    virtual void Clear();

    virtual void Draw(GScreen *screen);
};

class GWindow : public GContainer
{
protected:
    vector<ISelectable*> selectables;
    int selected_idx = -1;
    GScreen *screen;

public:

    GWindow(GScreen *scr);

    virtual void AddSelectable(ISelectable *selectable);

    //virtual void Draw(GScreen *screen);

    virtual void SelectPrev();
    virtual void SelectNext();

    virtual void UnfocusSelected();
    virtual void ActivateSelected();
    virtual void DeactivateSelected();
};

class GLabel : public GPanel
{
protected:
    char text[94] = {0};
    char *buffer = NULL;
    int text_margin_left = 2;
    int text_margin_top = 2;
    int text_scale = 1;
    int text_color = 0;
public:
    GLabel();

    void SetBuf(char *buf);

    virtual void SetText(const char* str);
    void SetTextMargin(int left, int top);
    void SetTextScale(int scale);
    int GetTextColor();
    void SetTextColor(int color);

    char* GetText();

    virtual void Draw(GScreen *screen);
};

class GButton : public GLabel//, public ISelectable
{
public:
    typedef void(*OnClickDelegate)(GButton* sender);
protected:
    Function *rtOnClick = NULL;
    OnClickDelegate onClick = NULL;

    int pressed_color = 0xCCCCCC;
    int pressed_border = 0xE5E5E5;

    int saved_color = 0xCCCCCC;
    int saved_border = 0xE5E5E5;

    int saved_select_border = 0xE5E5E5;

public:
    GButton()
    {
        border_color = 0xD0D0D0;
        back_color = 0xE0E0E0;
        border_width = 2;
        text_margin_top = 7;
        text_margin_left = 7;
        selectable = true;
        height = 23;
        width = 80;
    }

    void SetOnClickDelegate(OnClickDelegate delegate)
    {
        onClick = delegate;
    }

    void SetOnClickFunction(Function *func)
    {
        //scr->DrawText(200, 200, "ASSIGNED!");
        rtOnClick = func;
    }

    virtual void Draw(GScreen* scr)
    {
        GLabel::Draw(scr);
    }

    virtual void Select(GScreen *scr)
    {
        saved_select_border = border_color;
        border_color = 0x0000FF;
        GLabel::Draw(scr);
    }

    virtual void Unfocus(GScreen *scr)
    {
        border_color = saved_select_border;
        GLabel::Draw(scr);
    }

    virtual void Activate(GScreen* scr)
    {
        saved_border = border_color;
        saved_color = back_color;
        back_color = pressed_color;
        border_color = pressed_border;
        GLabel::Draw(scr);
        if(rtOnClick != NULL)
        {
            byte* sp = Runtime::Instance->GetStackPointer();
            *(uint*)++sp = (uint)this;

            sp += 4;
            *sp = (byte)Type::UI32;
            Runtime::Instance->SetStackPointer(sp);
            Runtime::Instance->Call(rtOnClick);
        }
        else if(onClick != NULL)
            onClick(this);
        //cout << "Clicked!\n";
        wait(250);
    }

    virtual void Deactivate(GScreen* scr)
    {
        back_color = saved_color;
        border_color = saved_border;
        GLabel::Draw(scr);
    }
};

class GEditText : public GButton
{
protected:
    int cursor = 0;
public:
    GEditText()
    {
        border_color = 0xD0D0D0;
        pressed_color = 0xFFFFFF;
        pressed_border = 0x0;//AFAFAF;
        border_width = 1;
        text_margin_top = 7;
        text_margin_left = 7;
        back_color = 0xF0F0F0;
        height = 25;
        width = 80;
        selectable = true;
    }

    virtual void SetText(const char *str)
    {
        GLabel::SetText(str);
        cursor = strlen(str);
    }

    virtual void Activate(GScreen* scr)
    {
        saved_border = border_color;
        saved_color = back_color;
        back_color = pressed_color;
        border_color = pressed_border;
        GLabel::Draw(scr);
        byte key = keyboard_getkey();
        while(key != KEY_ESC)
        {
            int val = keycode_to_ascii(key);
            if(val < 0)
            {
                key = keyboard_getkey();
                continue;
            }
            switch ((char)val) {
                case '\b':
                {
                    if(cursor > 0)
                        text[--cursor] = '\0';
                }
                    break;
                case '\n':
                {
                    if(onClick != NULL)
                        onClick(this);
                    return;
                }
                    break;
                default:
                {
                    text[cursor] = (char)val;
                    text[++cursor] = '\0';
                }
                    break;
            }
            GButton::Draw(scr);
            key = keyboard_getkey();
        }
    }
};

class GGraphView : public GPanel
{
public:
    typedef double (*Func)(double a, double b, double x);
protected:
    Func func = NULL;
    Function* rtFunc = NULL;
    int graph_color = 0;
    int lim_a = -5, lim_b = 5;
public:
    GGraphView()
    {
        back_color = 0xFFFFFF;
        border_color = 0x909090;
        border_width = 0;
        graph_color = 0xFF0000;
    }

    void SetGraphColor(int color)
    {
        graph_color = color;
    }

    void SetLimits(int a, int b)
    {
        lim_a = a;
        lim_b = b;
    }

    void SetFunction(Func f)
    {
        func = f;
    }

    void SetRtFunction(Function *f)
    {
        rtFunc = f;
    }

    double CallRt(double a, double b, double x)
    {
        return x*x*x - 2.0*x + 7.0;

        byte* sp = Runtime::Instance->GetStackPointer();
        *(double*)++sp = (double)x;

        sp += 8;
        *sp = (byte)Type::DOUBLE;
        Runtime::Instance->SetStackPointer(sp);
        Runtime::Instance->Call(rtFunc);
        sp = Runtime::Instance->GetStackPointer();
        double res = *(double*)(sp -= 8);
        Runtime::Instance->SetStackPointer(sp);
        return res;
    }

    virtual void Draw(GScreen *screen)
    {
        GPanel::Draw(screen);

        for(int i = 0; i < width; ++i)
        {
            screen->SetPixel(i+start.x, start.y+height/2, 0x909090);
        }

        if(func == NULL && rtFunc == NULL)
            return;

        for(int i = 0; i < lim_b-lim_a; ++i)
        {
            for(int y = start.y; y < start.y+height; ++y)
                screen->SetPixel(i*(width/(lim_b-lim_a)) + start.x, y, 0x909090);
        }

        double scale_x = width;//*(lim_b-lim_a);
        double scale_y = 6*(height/width);

        for(int i = 0; i < (int)width*(lim_b-lim_a); ++i)
        {
            double res;
            if(rtFunc == NULL)
                res = (-1)*scale_y*func(lim_a, lim_b, (lim_b - lim_a)*(i/scale_x));
            else
                res = (-1)*scale_y*CallRt(lim_a, lim_b, (lim_b - lim_a)*(i/scale_x));
            double y_point = start.y + height + res;
            int x = start.x + i/(lim_b-lim_a);

            if(y_point < start.y || y_point > start.y+height)
                continue;
            //if(x < start.x || x > start.x+width)
            //    continue;
            screen->SetPixel(x, (int)y_point, 0);
        }
    }
};

#endif // GELEMENTS_H
