#ifndef COMMON_COMMON_H
#define COMMON_COMMON_H

//extern "C"
//{
#include <string>
#include <new>
//}
#include "defines.h"

typedef unsigned char byte;

class Exception {
protected:
    kstd::string what;

public:
    virtual kstd::string What() {
        return what;
    }

    virtual ~Exception()
    {}
};

#endif // COMMON_H
