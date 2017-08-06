#include "gelements.h"
#include "interpolation.h"

extern "C"
{
    #include "io.h"
    #include "keyboard.h"
    #include "heap.h"
    #include "shutdown.h"
    #include "timer.h"

    void reboot();
}

void gui_routine(GScreen *screen);
void click_Reboot(GButton* sender);
void click_Shutdown(GButton* sender);
void button_Interpolate(GButton* sender);

GScreen *screen_ptr;

GEditText* inputX_ptr;
GEditText* inputAB_ptr;
GLabel* resultLabel_ptr;
GGraphView* graph_ptr;
GGraphView* interGraph_ptr;
GLabel *tablTitle_ptr;

char buf[1024];//"|N  |  x_o  | x_i |";

extern "C" void start_session(int height, int width)
{
    GScreen screen;

    screen.Init(vbe_lfb_addr, height, width);

    GPanel panel;
    panel.SetLocation(0, 0);
    panel.SetHeight(height);
    panel.SetWidth(width);

    panel.SetBackgroundColor(0xEEEEEE);
    panel.SetBorderWidth(0);

    GLabel title;
    title.SetLocation(345, 5);
    title.SetHeight(25);
    title.SetWidth(190);
    title.SetText("Newton interpolation\nSenko Andrew, IO-53");

    GLabel formula;
    formula.SetLocation(5, 25);
    formula.SetHeight(25);
    formula.SetWidth(200);
    formula.SetText("Formula: sin(x) - 2*cos(x)\na = 0, b = 4");

    GLabel requestX;
    requestX.SetLocation(5, 50);
    requestX.SetHeight(10);
    requestX.SetWidth(130);
    requestX.SetText("Enter x:");

    GEditText inputX;
    inputX.SetLocation(5, 65);
    inputX.SetHeight(23);
    inputX.SetWidth(80);

    GLabel requestAB;
    requestAB.SetLocation(140, 50);
    requestAB.SetHeight(10);
    requestAB.SetWidth(130);
    requestAB.SetText("Enter a, b:");

    GEditText inputAB;
    inputAB.SetLocation(140, 65);
    inputAB.SetHeight(23);
    inputAB.SetWidth(80);

    GButton calculate;
    calculate.SetLocation(5, 95);
    calculate.SetHeight(23);
    calculate.SetWidth(80);
    calculate.SetText("Calculate");
    calculate.SetOnClickDelegate(&button_Interpolate);

    GLabel resultLabel;
    resultLabel.SetLocation(5, 120);
    resultLabel.SetHeight(10);
    resultLabel.SetWidth(130);
    resultLabel.SetText("");

    GGraphView graph;
    graph.SetLocation(320, 25);
    graph.SetHeight(275);
    graph.SetWidth(475);

    GGraphView interGraph;
    interGraph.SetGraphColor(0x00FF00);
    interGraph.SetLocation(320, 305);
    interGraph.SetHeight(275);
    interGraph.SetWidth(475);

    GButton reboot_Button;
    reboot_Button.SetLocation(5, 570);
    reboot_Button.SetHeight(23);
    reboot_Button.SetWidth(80);
    reboot_Button.SetBackgroundColor(0x9999FF);
    reboot_Button.SetBorderColor(0xAAAAFF);
    reboot_Button.SetText("Reboot");
    reboot_Button.SetOnClickDelegate(&click_Reboot);

    GButton exit_Button;
    exit_Button.SetLocation(95, 570);
    exit_Button.SetHeight(23);
    exit_Button.SetWidth(80);
    exit_Button.SetBackgroundColor(0xFF9999);
    exit_Button.SetBorderColor(0xFFAAAA);
    exit_Button.SetText("Exit");
    exit_Button.SetOnClickDelegate(&click_Shutdown);

    GLabel tablTitle;
    tablTitle.SetLocation(5, 135);
    tablTitle.SetHeight(375);
    tablTitle.SetWidth(200);
    tablTitle.SetBuf(buf);

    screen.Add(&panel);
    screen.Add(&title);
    screen.Add(&formula);
    screen.Add(&requestX);
    screen.Add(&inputX);
    screen.Add(&requestAB);
    screen.Add(&inputAB);
    screen.Add(&calculate);
    screen.Add(&graph);
    screen.Add(&interGraph);
    screen.Add(&reboot_Button);
    screen.Add(&exit_Button);
    screen.Add(&tablTitle);

    screen_ptr = &screen;
    inputX_ptr = &inputX;
    inputAB_ptr = &inputAB;
    resultLabel_ptr = &resultLabel;
    graph_ptr = &graph;
    interGraph_ptr = &interGraph;
    tablTitle_ptr = &tablTitle;

    gui_routine(&screen);
}

void click_Shutdown(GButton* sender)
{
    shutdown();
}

void click_Reboot(GButton* sender)
{
    reboot();
}

void button_Interpolate(GButton* sender)
{
    resultLabel_ptr->SetText("");
    resultLabel_ptr->Draw(screen_ptr);

    const char* x_str = inputX_ptr->GetText();
    const char* ab_str = inputAB_ptr->GetText();

    char a[8] = {0};
    char b[8] = {0};
    char *val = a;

    int ptr = 0;

    for(int i = 0; i < strlen(ab_str); ++i)
    {
        if((ab_str[i] >= '0' && ab_str[i] <= '9') || ab_str[i] == '.' || ab_str[i] == '-')
        {
            val[ptr++] = ab_str[i];
        }
        else if(val == a)
        {
            val = b;
            ptr = 0;
        }
        else
            break;
    }

    double res = inter_main(atof(a), atof(b), atof(x_str));

    int sign= 1;

    if(res < 0)
    {
        sign = -1;
        res *= sign;
    }

    int ipart = (int)res;
    int rpart = (res - ipart) * 10000;

    char* res_str = "Result: \0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

    strcat(res_str, itoa(sign*ipart));
    strcat(res_str, ".");
    strcat(res_str, itoa(rpart));

    resultLabel_ptr->SetText(res_str);
    resultLabel_ptr->Draw(screen_ptr);

    res_str[8] = '\0';

    double da = atof(a);
    double db = atof(b);

    memset(buf, 0, 1024);
    strcpy(buf, "|N   |  y_o  | y_i |");

    //int row = 0;
    int count = (int)(db-da);

    for(int i = 0; i < count; ++i)
    {
        double orig = func_binder(da, db, (i+da)+0.5);
        double inter = inter_main(da, db, (i+da)+0.5);

        strcat(buf, "\n");
        strcat(buf, itoa(i+(int)da));
        strcat(buf, ".5: ");


        /*if(orig < 0)
        {
            sign = -1;
            orig *= sign;
        }*/

        //int _rpart;
        int sign = 1;

        if(orig < 0)
        {
            sign = -1;
            orig *= sign;
        }

        int _ipart = (int)orig;
        int _rpart = (orig - _ipart) * 10000;

        strcat(buf, itoa(sign*_ipart));
        strcat(buf, ".");
        strcat(buf, itoa(_rpart));
        strcat(buf, ", ");

        sign = 1;

        if(inter < 0)
        {
            sign = -1;
            inter *= sign;
        }

        _ipart = (int)inter;
        _rpart = (inter - _ipart) * 10000;

        strcat(buf, itoa(sign*_ipart));
        strcat(buf, ".");
        strcat(buf, itoa(_rpart));
    }

    tablTitle_ptr->Draw(screen_ptr);

    graph_ptr->SetLimits((int)da, (int)db);
    graph_ptr->SetFunction(&func_binder);
    graph_ptr->Draw(screen_ptr);

    interGraph_ptr->SetLimits((int)da, (int)db);
    interGraph_ptr->SetFunction(&inter_main);
    interGraph_ptr->Draw(screen_ptr);
}

void gui_routine(GScreen *screen)
{
    /*while(true)
    {
        int code = keyboard_getkey();
        // up   == 72
        // down == 80
        // esc  == 1
        switch (code) {
            case KEY_UP:
                screen->SelectPrev();
                break;
            case KEY_DOWN:
                screen->SelectNext();
                break;
            case KEY_ESC:
                //reboot();
                screen->UnfocusSelected();
                break;
            case KEY_ENTER:
                screen->ActivateSelected();
                //wait(250);
                screen->DeactivateSelected();
                break;
            default:
                break;
        }
    }*/
}

extern "C" void __cxa_pure_virtual()
{
    uint8_t good = 0x02;
//    u8 b;
    while (good & 0x02)
        good = inb(0x64);
    outb(0x64, 0xFE);
    __asm volatile ("hlt");
}
