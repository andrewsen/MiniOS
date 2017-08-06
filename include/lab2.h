#ifndef LAB2_H
#define LAB2_H

typedef struct
{
    long ** matrix;
    int rows, cols;
} matrix_t;

void fill(matrix_t *matr);
matrix_t mul(matrix_t a, matrix_t b);
void print(matrix_t matr);
long sumMinInColumns(matrix_t matr);
int main_lab2(void);

double get_double_f();
float get_float_f();

#endif
