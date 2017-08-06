#ifndef GUI_H
#define GUI_H

extern "C"
{
    #include "screen.h"
}

class GScreen;

class ISelectable
{
protected:
    bool selectable = false;
public:
    virtual void Select(GScreen *scr) {}
    virtual void Activate(GScreen *scr) {}
    virtual void Deactivate(GScreen *scr) {}
    virtual void Unfocus(GScreen *scr) {}

    bool IsSelectable();
    void IsSelectable(bool sel);
};

class GElement : public ISelectable
{
protected:
    //bool selectable = false;
    char name[80];
public:
    GElement();
    virtual void Draw(GScreen *scr) = 0;
    char* GetName() { return name; }
    //bool IsSelectable() { return selectable; }
};

class GScreen
{
    ulong base;
    int height, width;
    GElement **children;
    int children_count;
public:
    GScreen();

    void Init(ulong gbase, int height, int width);

    void Add(GElement *element);

    void Redraw();

    void DrawText(int x, int y, const char* text);

    void SetPixel(int x, int y, int color);
    int GetPixel(int x, int y);

    int GetHeight() { return height; }
    int GetWidth() { return width; }
};

#endif // GUI_H
