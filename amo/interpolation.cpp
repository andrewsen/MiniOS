#include <cprintf>
#include <ctypes>
#include <cheap>
#include "interpolation.h"
#include <vector>
#include <string>
#include <new>
#include <fstream>

using namespace kstd;

struct Point
{
    double x;
    double y;
};

Point points[N];

double func_binder(double a, double b, double x)
{
    return function(x);
}

double inter_main(double a, double b, double x)
{
    //cin >> a >> b >> x;
    ifstream ifs("/hello.txt");

    //char text[512] = {0};
    //ext2_read_file(0, 0, (byte*)file, (byte*)text, &num, 0);
    //ifs.read(text, 3);

    kstd::string theory = "str test: ";
    cout << theory << endl;
    ifs.close();
    int max = 0;
    cin >> max;

    kstd::vector<int> vec;
    for(int i = 0; i < max; ++i)
    {
        vec.push_back(i);
        printf("vec val: %d, %d elems\n", vec[i], vec.capacity());
    }
    return 42.0;


    for(int i = 0; i < N; ++i)  // TEST for N+1
    {
        double h = (b - a)/N;
        double xi = a + h*i;

        points[i] = Point{xi, function(xi)};
    }

    return interpolate(x, N-1);
}

double interpolate(double x, int i)
{
    double res = sep(0, i);
    if(i == 0)
        return res;
    for(int j = 0; j < i; ++j)
        res *= x - points[j].x;
    return res + interpolate(x, i-1);
}

double sep(double beg, double p)
{
    if(beg == p)
        return points[(int)beg].y;
    return (sep(beg+1, p) - sep(beg, p-1)) / (points[(int)p].x - points[(int)beg].x);
}

double function(double x)
{
    double x1 = x;
    double x2 = x;
    ASMV ("fldl %0;"
         "fsin;"
         "fstpl %0" : "+m"(x1));
    ASMV ("fldl %0;"
         "fcos;"
         "fstpl %0" : "+m"(x2));

    return x1 - 2*x2;
}
