#include <runtime_graphics.h>
#include <json/json.h>

#include <fstream>

using namespace kext;
using namespace kstd;

Graphics *graphics = nullptr;
GWindow *window = nullptr;

void createChildren(const JSONArray &children, GContainer *parent);
void createContainer(const JSONObject &json, GContainer *container);
void createPanel(const JSONObject &json, GPanel *panel);
void createLabel(const JSONObject &json, GLabel *label);
void createSelectable(const JSONObject &json, ISelectable *selectable);
void createButton(const JSONObject &json, GButton *button);
void createEditText(const JSONObject &json, GEditText *editText);
void createGraph(const JSONObject &json, GGraphView *graph);

void initUI(const string &json_file)
{
    ifstream ifs(json_file.c_str());
    if(!ifs)
    {
        panic(("UI file `" + json_file + "` not found").c_str(), 2, -1);
        return;
    }

    int size = ifs.size();

    char *json = new char[size+1];
    ifs.read(json, size);
    json[size] = '\0';

    JSONDoc doc;
    doc.FromString(json);
    if(!doc.IsObject())
        panic("UI root element should be an object", 2, -1);

    JSONObject& root = doc.Obj();
    int height = root["mode"].Obj()["H"].Val().ToInt();
    int width = root["mode"].Obj()["W"].Val().ToInt();

    cout << height << "x" << width << endl;

    //wait_s(5);

    graphics = new Graphics;
    graphics->screen = new GScreen;

    window = new GWindow(graphics->screen);
    window->SetLocation(0,0);
    window->SetHeight(height);
    window->SetWidth(width);
    window->SetBackgroundColor(0xA0A0A0);

    createChildren(root["window"].Arr(), window);

    //wait_s(5);
    graphics->screen->Init(0, height, width);
    graphics->screen->Add(window);

    cout << "File loaded!" << endl;
    //return;

    while(true)
    {
        int code = keyboard_getkey();
        // up   == 72
        // down == 80
        // esc  == 1
        switch (code) {
            case KEY_UP:
                window->SelectPrev();
                break;
            case KEY_DOWN:
                window->SelectNext();
                break;
            case KEY_ESC:
                //reboot();
                window->UnfocusSelected();
                break;
            case KEY_ENTER:
                window->ActivateSelected();
                wait(250);
                window->DeactivateSelected();
                break;
            default:
                break;
        }
    }
}

void createChildren(const JSONArray &children, GContainer *parent)
{
    //cout << "begin - createChildren" << endl;
    for(auto e : children)
    {
        if(!e->IsObject())
            panic("UI element should be an object", 2, -1);

        auto type = e->Obj()["type"].Val().ToString();

        GElement *elem;

        if(type == "Panel")
        {
            elem = new GPanel;
            cout << "creating Panel\n";
            createPanel(e->Obj(), (GPanel*)elem);
        }
        else if(type == "Label")
        {
            elem = new GLabel;
            cout << "creating Label\n";
            createLabel(e->Obj(), (GLabel*)elem);
        }
        else if(type == "Button")
        {
            elem = new GButton;
            cout << "creating Button\n";
            createButton(e->Obj(), (GButton*)elem);
            if(((GButton*)elem)->IsSelectable())
            {
                //cout << "Adding Button to selectables\n";
                window->AddSelectable((ISelectable*)elem);
            }
        }
        else if(type == "LineEdit")
        {
            elem = new GEditText;
            cout << "creating LineEdit\n";
            createEditText(e->Obj(),(GEditText*)elem);
            if(((GEditText*)elem)->IsSelectable())
            {
                //cout << "Adding LineEdit to selectables\n";
                window->AddSelectable((ISelectable*)elem);
            }
        }
        else if(type == "Grid")
        {
            elem = new GContainer;
            cout << "creating Grid\n";
            createContainer(e->Obj(), (GContainer*)elem);
        }
        else if(type == "Graph")
        {
            elem = new GGraphView;
            createGraph(e->Obj(), (GGraphView*)elem);
        }

        parent->AddElement(elem);
        if(e->Obj().exists("name"))
        {
            graphics->namedElements.push_back(
                        pair<string, GElement*>(e->Obj()["name"].Val().ToString(), elem)
                    );
        }
    }
    //cout << "end - createChildren" << endl;
}

void createContainer(const JSONObject &json, GContainer *container)
{
    //cout << "begin - createContainer" << endl;
    createPanel(json, container);
    if(json.exists("children"))
    {
        createChildren(json["children"].Arr(), container);
    }
    //cout << "end - createContainer" << endl;
}

void createPanel(const JSONObject &json, GPanel *panel)
{
    //cout << "begin - createPanel" << endl;
    int X = 0;
    int Y = 0;
    bool matchX = false;
    bool matchY = false;
    if(json.exists("height"))
    {
        auto height = json["height"].Val();
        if(height.IsString() && height.ToString() == "match")
        {
            panel->SetHeight(window->GetHeight());
            matchX = true;
        }
        else// if(height.IsInt())
        {
            panel->SetHeight(height.Val().ToInt());
        }
    }

    if(json.exists("width"))
    {
        auto width = json["width"].Val();
        if(width.IsString() && width.ToString() == "match")
        {
            panel->SetWidth(window->GetWidth());
            matchY = true;
        }
        else// if(width.IsInt())
        {
            panel->SetWidth(width.Val().ToInt());
        }
    }

    if(json.exists("position"))
    {
        auto position = json["position"].Val().ToString();
        char x[5] = {0};
        char y[5] = {0};

        char *cur = x;
        int pos = 0;
        for(char ch : position)
        {
            if(ch == ' ')
            {
                if(cur == y)
                    break;
                pos = 0;
                cur = y;
            }
            else if(ch == '\0')
                break;
            else
                cur[pos++] = ch;
        }

        X = atoi(x);

        if(cur == x)
            Y = X;
        else
            Y = atoi(y);
    }

    if(json.exists("color"))
    {
        int color = 0xFFFFFF;
        auto str_color = json["color"].Val().ToString();
        if(str_color[0] != '#')
            panic("UI: invalid color", 2, -1);
        const char* val = str_color.c_str()+1;

        color = hatoi(val);
        panel->SetBackgroundColor(color);
        //delete val; // TODO FIX LEAKS
    }
    if(json.exists("borderColor"))
    {
        auto str_color = json["borderColor"].Val().ToString();
        if(str_color[0] != '#')
            panic("UI: invalid color", 2, -1);
        const char* val = str_color.c_str()+1;

        panel->SetBorderColor(hatoi(val));
        //delete val; // TODO FIX LEAKS
    }
    if(json.exists("borderWidth"))
    {
        auto width = json["borderWidth"].Val().ToString();
        const char* val = width.c_str();

        panel->SetBorderWidth(atoi(val));
        //delete val; // TODO FIX LEAKS
    }

    if(!matchX)
        panel->SetX(X);
    if(!matchY)
        panel->SetY(Y);
    cout << "panel: {" << X << "," << Y << "}={" << panel->GetHeight() << "," << panel->GetWidth() << "}\n";
    //cout << "end - createPanel" << endl;
}

void createLabel(const JSONObject &json, GLabel *label)
{
    //cout << "begin - createLabel" << endl;
    createPanel(json, label);

    int textMarginX = 2;
    int textMarginY = 2;

    if(json.exists("textMargin"))
    {
        auto position = json["textMargin"].Val().ToString();
        char x[5] = {0};
        char y[5] = {0};

        char *cur = x;
        int pos = 0;
        for(char ch : position)
        {
            if(ch == ' ')
            {
                if(cur == y)
                    break;
                pos = 0;
                cur = y;
            }
            else if(ch == '\0')
                break;
            else
                cur[pos++] = ch;
        }

        textMarginX = atoi(x);

        if(cur == x)
            textMarginY = textMarginX;
        else
            textMarginY = atoi(y);
    }
    if(json.exists("textColor"))
    {
        int color = 0;
        auto str_color = json["textColor"].Val().ToString();
        if(str_color[0] != '#')
            panic("UI: invalid color", 2, -1);
        const char* val = str_color.c_str()+1;

        color = hatoi(val);
        label->SetTextColor(color);
        //delete val; // TODO FIX LEAKS
    }

    if(json.exists("text"))
    {
        auto text = json["text"].Val().ToString().c_str();
        label->SetText(text);
    }
    //cout << "end - createLabel" << endl;
}

void createSelectable(const JSONObject &json, ISelectable *selectable)
{
    if(json.exists("selectable"))
    {
        //cout << "selectable marked\n";
        selectable->IsSelectable(json["selectable"].Val().ToBool());
    }
}

void createButton(const JSONObject &json, GButton *button)
{
    //cout << "begin - createButton" << endl;
    createLabel(json, button);
    //cout << "button - selectable:" << button->IsSelectable() << endl;
    //createSelectable(json, button);

    if(json.exists("onClick"))
    {
        //auto func_sign = json["onClick"].Val().ToString();

        RTType args [1];
        args[0].dimens = 0;
        args[0].plain = Type::UI32;

        //cout << "\tbutton: before FindFunc\n";
        Function* func = graphics->rt->FindFunction(json["onClick"].Val().ToString().c_str(), args, 1);
        if(func != nullptr)
            cout << "\tbutton found!\n";
        else
            cout << "\tbutton not found!\n";
        //wait_s(5);

        button->SetOnClickFunction(func);
    }
    //cout << "end - createButton" << endl;
}

void createEditText(const JSONObject &json, GEditText *editText)
{
    //cout << "editText - selectable:" << editText->IsSelectable() << endl;
    createButton(json, editText);
    //createSelectable(json, editText);
    //cout << "end - createEditText" << endl;
}

void createGraph(const JSONObject &json, GGraphView *graph)
{
    createPanel(json, graph);

    if(json.exists("graphColor"))
    {
        int color = 0;
        auto str_color = json["graphColor"].Val().ToString();
        if(str_color[0] != '#')
            panic("UI: invalid color", 2, -1);
        const char* val = str_color.c_str()+1;

        color = hatoi(val);
        graph->SetGraphColor(color);
        //delete val; // TODO FIX LEAKS
    }
    if(json.exists("func"))
    {
        //auto func_sign = json["onClick"].Val().ToString();

        RTType args [1];
        args[0].dimens = 0;
        args[0].plain = Type::DOUBLE;

        //cout << "\tbutton: before FindFunc\n";
        Function* func = graphics->rt->FindFunction(json["func"].Val().ToString().c_str(), args, 1);
        if(func != nullptr)
            cout << "\tbutton found!\n";
        else
            cout << "\tbutton not found!\n";
        //wait_s(5);

        graph->SetRtFunction(func);
    }
}

void internalFindElement(Runtime* rt, Function *f, byte* fargs)
{
    char* name = (char*)(*(size_t*)fargs + rt->ARRAY_METADATA_SIZE);
    uint id = 0;

    for(auto& elem : graphics->namedElements)
    {
        if(elem.first == name)
        {
            id = (uint)elem.second;
            break;
        }
    }
    *(uint*)++rt->stack_ptr = id;

    rt->stack_ptr += 4;
    *rt->stack_ptr = (byte)Type::UI32;
}

void internalGetName(Runtime* rt, Function *f, byte* fargs)
{
    //char* name = (char*)(*(size_t*)fargs + rt->ARRAY_METADATA_SIZE);
    uint id = *(uint*)fargs;
    string name;

    for(auto& elem : graphics->namedElements)
    {
        if((uint)elem.second == id)
        {
            name = elem.first;

            uint len = name.size()+1;
            byte* addr = rt->memoryManager.Allocate(len + rt->ARRAY_METADATA_SIZE);
            *addr = (byte)Type::UTF8;
            *(uint*)(addr+1) = len;
            *(size_t*)++rt->stack_ptr = (uint)addr;
            strcpy((char*)(addr + rt->ARRAY_METADATA_SIZE), name.c_str());

            //rt->mem_first_free += strlen((char*)rt->mem_first_free) + 1 + rt->ARRAY_METADATA_SIZE;

            rt->stack_ptr += 4;
            *rt->stack_ptr = (byte)Type::UTF8;
            return;
        }
    }
    *(uint*)++rt->stack_ptr = 0;

    rt->stack_ptr += 4;
    *rt->stack_ptr = (byte)Type::PTR_NULL;
}

void internalGetText(Runtime* rt, Function *f, byte* fargs)
{
    //char* name = (char*)(*(size_t*)fargs + rt->ARRAY_METADATA_SIZE);
    uint id = *(uint*)fargs;
    GLabel *label = (GLabel*)id;

    *(uint*)++rt->stack_ptr = (uint)rt->memoryManager.AllocateString(label->GetText());

    rt->stack_ptr += 4;
    *rt->stack_ptr = (byte)Type::UTF8;

    //graphics->screen->DrawText(250, 250, label->GetText());
}

void internalSetText(Runtime* rt, Function *f, byte* fargs)
{
    //graphics->screen->DrawText(200, 200, "LOOOOL");
    uint id = *(uint*)fargs;
    char* text = (char*)(*(size_t*)(fargs+5) + rt->ARRAY_METADATA_SIZE);


    GLabel *label = (GLabel*)id;
    label->SetText(text);
    graphics->screen->Redraw();
}

void internalGetColor(Runtime* rt, Function *f, byte* fargs)
{
    uint id = *(uint*)fargs;
    GPanel *elem = (GPanel*)id;

    *(uint*)++rt->stack_ptr = elem->GetBackgroundColor();

    rt->stack_ptr += 4;
    *rt->stack_ptr = (byte)Type::UI32;
}

void internalSetColor(Runtime* rt, Function *f, byte* fargs)
{
    uint id = *(uint*)fargs;
    GPanel *elem = (GPanel*)id;

    elem->SetBackgroundColor(*(uint*)(fargs+5));
    graphics->screen->Redraw();
}

void internalGetTextColor(Runtime* rt, Function *f, byte* fargs)
{
    uint id = *(uint*)fargs;
    GLabel *label = (GLabel*)id;

    *(uint*)++rt->stack_ptr = label->GetTextColor();

    rt->stack_ptr += 4;
    *rt->stack_ptr = (byte)Type::UI32;
}

void internalSetTextColor(Runtime* rt, Function *f, byte* fargs)
{
    uint id = *(uint*)fargs;
    GLabel *label = (GLabel*)id;

    label->SetTextColor(*(uint*)(fargs+5));
    graphics->screen->Redraw();
}

void internalGetPos(Runtime* rt, Function *f, byte* fargs)
{
    uint id = *(uint*)fargs;
    GPanel *elem = (GPanel*)id;

    byte* result = rt->memoryManager.AllocateArray(Type::UI32, 2);

    *(uint*)(result+Runtime::ARRAY_METADATA_SIZE) = elem->GetLocation().x;
    *(uint*)(result+Runtime::ARRAY_METADATA_SIZE) = elem->GetLocation().y;

    *(uint*)++rt->stack_ptr = (uint)result;

    rt->stack_ptr += 4;
    *rt->stack_ptr = (byte)Type::ARRAY;
}

void internalSetPos(Runtime* rt, Function *f, byte* fargs)
{
    uint id = *(uint*)fargs;
    GPanel *elem = (GPanel*)id;

    elem->SetLocation(*(uint*)(fargs+9), *(uint*)(fargs+5));
    graphics->screen->Redraw();
}

void internalInit(Runtime* rt, Function *f, byte* fargs)
{
    if(graphics != nullptr)
    {
        delete graphics->screen;
        delete graphics;
    }

    graphics = new Graphics;
    graphics->screen = new GScreen;
    graphics->rt = Runtime::Instance;

    graphics->screen->Init(0, *(uint*)(fargs), *(uint*)(fargs+5));
}

void internalLoadUI(Runtime* rt, Function *f, byte* fargs)
{
    //cout << "--begin load ui\n";
    char* json_file = (char*)(*(size_t*)(fargs) + rt->ARRAY_METADATA_SIZE);

    ifstream ifs(json_file);
    if(!ifs)
    {
        panic(("UI file `" + string(json_file) + "` not found").c_str(), 2, -1);
        return;
    }

    int size = ifs.size();

    char *json = new char[size+1];
    ifs.read(json, size);
    json[size] = '\0';

    JSONDoc doc;
    doc.FromString(json);
    if(!doc.IsObject())
        panic("UI root element should be an object", 2, -1);

    JSONObject& root = doc.Obj();
    int height = graphics->screen->GetHeight();
    int width = graphics->screen->GetWidth();

    cout << "Layout: " << height << "x" << width << endl;


    window = new GWindow(graphics->screen);
    window->SetLocation(0,0);
    window->SetHeight(height);
    window->SetWidth(width);
    window->SetBackgroundColor(0xA0A0A0);

    createChildren(root["window"].Arr(), window);

    graphics->screen->Add(window);
    //cout << "--end load ui\n";

}

void internalExec(Runtime* rt, Function *f, byte* fargs)
{
    while(true)
    {
        int code = keyboard_getkey();
        // up   == 72
        // down == 80
        // esc  == 1
        switch (code) {
            case KEY_UP:
                window->SelectPrev();
                break;
            case KEY_DOWN:
                window->SelectNext();
                break;
            case KEY_ESC:
                //reboot();
                window->UnfocusSelected();
                break;
            case KEY_ENTER:
                window->ActivateSelected();
                wait(250);
                window->DeactivateSelected();
                break;
            default:
                break;
        }
    }
}
