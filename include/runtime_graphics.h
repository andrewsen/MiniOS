#ifndef RUNTIME_GRAPHICS_H
#define RUNTIME_GRAPHICS_H

#include <string>
#include <gui.h>
#include <runtime.h>
#include <gelements.h>
#include <vector>
#include <string>
#include <pair>

struct Graphics
{
    kstd::vector<kstd::pair<kstd::string, GElement *>> namedElements;
    GScreen *screen;
    Runtime *rt;
};

void initUI(const kstd::string &json_file);

#endif // RUNTIME_GRAPHICS_H
