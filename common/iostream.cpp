#include <iostream>

namespace kstd {
    istream cin;
    ostream cout;
    ostream clog;
    ostream cerr;

ostream& endl(ostream& str)
{
    str << "\n";
    return str;
}
}
