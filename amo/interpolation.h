#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#define N 10

double func_binder(double a, double b, double x);
#ifdef __cplusplus
extern "C" double inter_main(double a, double b, double x);
#else
double inter_main(double a, double b, double x);
#endif
double sep(double beg, double p);
double interpolate(double x, int i);
double function(double x);

#endif // INTERPOLATION_H
