#include "gelements.h"
#include "charmap.h"
//#include <vector>

#include <cstring>

GElement::GElement()
{

}

GLabel::GLabel()
{
    //back_color = 0xEEEEEE;
    height = 18;
    width = 80;
}

void GLabel::SetBuf(char *buf)
{
    buffer = buf;
}

void GLabel::SetText(const char *str)
{
    strcpy(text, str);
}

void GLabel::SetTextMargin(int left, int top)
{
    text_margin_left = left;
    text_margin_top = top;
}

void GLabel::SetTextScale(int scale)
{
    text_scale = scale;
}

int GLabel::GetTextColor() {
    return text_color;
}

void GLabel::SetTextColor(int color) {
    text_color = color;
}

char *GLabel::GetText()
{
    return text;
}

void GLabel::Draw(GScreen *screen)
{
    GPanel::Draw(screen);

    char ch;
    int char_pos = 0;
    int char_line = 0;

    char *syms;
    if(buffer == NULL)
        syms = text;
    else
        syms = buffer;

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
                    if((text_margin_left + char_pos + j) >= width - border_width)
                        return;
                    screen->SetPixel(start.x + text_margin_left + char_pos + j,
                                     start.y + text_margin_top + char_line + i,
                                     text_color);
                }
                //else
                //    screen->SetPixel(start.x + char_pos + j, start.y + i, 0xFFFFFF);
            }
        }

        char_pos += CHAR_WIDTH + 2;
    }
}

GWindow::GWindow(GScreen *scr)
{
    screen = scr;
}

void GWindow::AddSelectable(ISelectable *selectable)
{
    selectables.push_back(selectable);
}

void GWindow::SelectPrev()
{
    /*if(selected_idx != -1)
        children[selected_idx]->Unfocus(this);
    else
        selected_idx = children_count;
    bool was_selected = false;
    for(int i = selected_idx-1; i >= 0; --i)
    {
        if(children[i]->IsSelectable())
        {
            selected_idx = i;
            was_selected = true;
            children[i]->Select(this);
            break;
        }
    }
    if(!was_selected)
    {
        if(selected_idx == -1)
            return;

        for(int i = children_count-1; i > selected_idx; --i)
        {
            if(children[i]->IsSelectable())
            {
                selected_idx = i;
                children[i]->Select(this);
                break;
            }
        }
    }*/
    cout << "UP: Current selection << " << selected_idx << endl;
    if(selected_idx != -1)
        selectables[selected_idx]->Unfocus(screen);
    else if(selectables.size() != 0)
    {
        selected_idx = selectables.size()-1;
        selectables[selected_idx]->Select(screen);
        return;
    }
    else
        return;

    if(selected_idx > 0)
        selectables[--selected_idx]->Select(screen);
    else
    {
        selected_idx = selectables.size()-1;
        selectables[selected_idx]->Select(screen);
    }
}

void GWindow::SelectNext()
{
    cout << "DOWN: Current selection << " << selected_idx << endl;
    if(selected_idx != -1)
        selectables[selected_idx]->Unfocus(screen);
    else if(selectables.size() != 0)
    {
        selected_idx = 0;
        selectables[selected_idx]->Select(screen);
        return;
    }
    else
        return;

    if(selected_idx < selectables.size()-1)
        selectables[++selected_idx]->Select(screen);
    else
    {
        selected_idx = 0;
        selectables[selected_idx]->Select(screen);
    }
    /*for(int i = selected_idx+1; i < selectables.size(); ++i)
    {
        if(children[i]->IsSelectable())
        {
            selected_idx = i;
            was_selected = true;
            children[i]->Select(this);
            break;
        }
    }*/
}

void GWindow::UnfocusSelected()
{
    if(selected_idx != -1)
        selectables[selected_idx]->Unfocus(screen);
}

void GWindow::ActivateSelected()
{
    if(selected_idx != -1)
        selectables[selected_idx]->Activate(screen);
}

void GWindow::DeactivateSelected()
{
    if(selected_idx != -1)
        selectables[selected_idx]->Deactivate(screen);
}

void GContainer::AddElement(GElement *elem)
{
    elements.push_back(elem);
}

void GContainer::RemoveElement(GElement *elem)
{
    elements.remove(elem);
}

void GContainer::Clear()
{
    //for(auto elem : elements)
    //    delete elem;
    elements.clear();
}

void GContainer::Draw(GScreen *screen)
{
    GPanel::Draw(screen);
    for(auto elem : elements)
    {
        elem->Draw(screen);
    }
}
