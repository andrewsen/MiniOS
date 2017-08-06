#ifndef LOG_H
#define LOG_H

#include <iostream>
//#include <fstream>
//#include <typeinfo>
#include "string.h"

using namespace kstd;


class Log
{
    char out[128];
public:
    enum LogType
    {
        Error, Warning, Info
    };
    Log();

    void SetType(LogType t)
    {
        ltype = t;
    }

    template <typename T> Log &operator<<(T arg)
    {
        if(ltype == Info)
            clog << arg;
        else
            cerr << arg;

        return *this;
    }

    void Close()
    {

    }

private:

    LogType ltype;
};

#endif // LOG_H
