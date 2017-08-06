#include "tui.hpp"
extern "C" {
    #include "timer.h"
}

TScreen TScreen::screen;
int TElement::counter = 1;

void b_click(TElement * element);

extern "C" void redraw() {
    TScreen * screen = TScreen::getScreen();
    //wait_s(5);
    screen->initialize();

    printf("Init TUI 4\n");
    wait_s(2);
    TWindow window;
    printf("Init TUI 5\n");
    wait_s(2);

    window.setName("mainWindow");
    window.setHeigh(MAX_ROW);
    window.setWidth(MAX_COL);
    window.setPos(TPoint(0, 0));
    window.setBackColor(Green);

    screen->addElement(&window);

    int uids[5];

    char * ids [5]= {"1", "2", "3", "4", "5"};
    {
        TButton btn = TButton();
        TButton btn2 = TButton();
        TButton btn3 = TButton();
        btn.setHeigh(5);
        btn.setWidth(26);
        btn.setPos(TPoint(20,1));
        btn.setBackColor(Blue);

        btn2.setHeigh(5);
        btn2.setWidth(26);
        btn2.setPos(TPoint(23,4));
        btn2.setBackColor(Pink);

        btn3.setHeigh(5);
        btn3.setWidth(26);
        btn3.setPos(TPoint(26,7));
        btn3.setBackColor(Yellow);

        screen->addElement(&btn);
        screen->addElement(&btn2);
        screen->addElement(&btn3);

        uids[0] = btn.getUid();
        uids[1] = btn2.getUid();
        uids[2] = btn3.getUid();
    }
    wait_s(5);
    screen->removeElement(uids[0]);
    wait_s(1);
    screen->removeElement(uids[1]);
    wait_s(1);
    screen->removeElement(uids[2]);
    wait_s(1);
    screen->removeElement(&window);
    screen->dispose();
    /*
    for (int i = 0; i < 5; i++) {
        char name [10] = "button_";
        strcat(name, ids[i]);

        TButton b = TButton(name);
        b.setHeigh(5);
        b.setWidth(26);
        b.setPos(TPoint(27,5+i));

        screen->addElement(&b);
        uids[i] = b.getUid();
        //screen->setCell(cell, 1, 1);
        //screen->redraw();
        //TButton b("test");
       // b.onClick(b_click);
        //b.Click();
    }

    for (int i = 4; i != 0; i--) {
        screen->removeElement(uids[i]);
    }*/

    //screen->setCell(cell, 1, 1);
    //screen->redraw();
    //TButton b("test");
    //b.onClick(b_click);
    //b.Click();
}

void b_click(TElement * element) {
    //printf("C++ clicked!\n");
}

colors TElement::getBackColor() const
{
    return BackColor;
}

void TElement::setBackColor(const colors &value)
{
    BackColor = value;
}

colors TElement::getForeColor() const
{
    return ForeColor;
}

void TElement::setForeColor(const colors &value)
{
    ForeColor = value;
}

TElement::event TElement::getOnDispose() const
{
    return onDispose;
}//TScreen screen

void TElement::setOnDispose(const event &value)
{
    onDispose = value;
}

TElement::event TElement::getOnInitialize() const
{
    return onInitialize;
}

void TElement::setOnInitialize(const event &value)
{
    onInitialize = value;
}
