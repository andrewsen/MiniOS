#include "lab2.h"
#include "stdlib.h"

#define VARIANT 5324

const int C5 = VARIANT % 5;
const int C7 = VARIANT % 7;
const int C11 = VARIANT % 11;

double get_double_f()
{
    return 0.5;
}

float get_float_f()
{
    return 0.5f;
}

void fill(matrix_t* matr)
{
    printf("Enter number of rows and columns:\n");
    char buf[10];
    scanf(buf);
    matr->rows = str_to_int(buf);
    scanf(buf);
    matr->cols = str_to_int(buf);
    //scanf("%ld %ld", &matr->rows, &matr->cols);

    matr->matrix = (long*)kmalloc(matr->rows*sizeof(long*));
    printf("Enter elements:\n");
    for(int i = 0; i < matr->rows; ++i)
    {
        matr->matrix[i] = (long)kmalloc(matr->cols*sizeof(long));
        for(int j = 0; j < matr->cols; ++j)
        {
            scanf(buf);
            matr->matrix[i][j] = str_to_long(buf);
        }
    }
}

matrix_t mul(matrix_t a, matrix_t b)
{
    if(a.cols != b.rows)
    {
        panic("Number of rows in matrix A must be equals to number of columns in B\n", 3, NULL);
    }
        //throw new MatrixMultiplyException();
    matrix_t res;
    res.rows = a.rows;
    res.cols = b.cols;
    res.matrix = (long*)kmalloc(a.rows*sizeof(long*));

    for(int i = 0; i < a.rows; ++i)
    {
        res.matrix[i] = (long)kmalloc(a.cols*sizeof(long));
        for(int j = 0; j < b.cols; ++j)
        {
            res.matrix[i][j] = 0;
            for(int n = 0; n < a.cols; ++n)
            {
                res.matrix[i][j] += a.matrix[i][n] * b.matrix[n][j];
            }
        }
    }

    return res;
}

void print(matrix_t matr)
{
    for(int i = 0; i < matr.rows; ++i)
    {
        for(int j = 0; j < matr.cols; ++j)
        {
            printf("%ld ", matr.matrix[i][j]);
        }
        printf("\n");
    }
}

long sumMinInColumns(matrix_t matr)
{
    long sum = 0;
    for(int i = 0; i < matr.rows; ++i)
    {
        long t = matr.matrix[0][i];
        for(int j = 0; j < matr.cols; ++j)
        {
            if(matr.matrix[j][i] < t)
                t = matr.matrix[j][i];
        }
        sum += t;
    }
    return sum;
}

int main_lab2(void)
{
    printf("Variant: %d, C5: %d, C7: %d, C11: %d\n", VARIANT, C5, C7, C11);
    matrix_t a;
    fill(&a);
    matrix_t b;
    fill(&b);

    matrix_t c;
    printf("С = A * B = \n");
    c = mul(a, b);
    print(c);
    printf("Sum of minimal elements in C's columns: %ld\n", sumMinInColumns(c));
    return 0;
}

/*
import java.lang.*;
import java.util.ArrayList;
import java.util.Scanner;

class MatrixMultiplyException extends Exception
{
    public MatrixMultiplyException()
    {
        super("Number of rows in matrix A must be equals to number of columns in B");
    }
}

class Matrix
{
    //ArrayList<ArrayList<Long>> matrix = new ArrayList<ArrayList<Long>>();
    long[][] matrix;
    int rows, cols;

    void fill()
    {
        Scanner s = new Scanner(System.in);
        System.out.println("Enter number of rows and columns:");
        rows = s.nextInt();
        cols = s.nextInt();
        matrix = new long[rows][cols];
        //System.out.println("Rows: " + rows);
        //System.out.println("Columns: " + cols);
        System.out.println("Enter elements:");
        for(int i = 0; i < rows; ++i)
        {
            //long[] row = new long[cols];//ArrayList<Long>();
            for(int j = 0; j < cols; ++j)
            {
                matrix[i][j] = s.nextLong();
            }
        }
        /*for(ArrayList<Long> col : matrix)
        {
            for(long elem : col)
            {
                System.out.print(elem + " ");
            }
            System.out.print("\n");
        }* /
    }

    Matrix mul(Matrix m) throws MatrixMultiplyException
    {
        if(cols != m.rows)
            throw new MatrixMultiplyException();
        Matrix res = new Matrix();
        res.rows = rows;
        res.cols = m.cols;
        res.matrix = new long[res.rows][res.cols];

        for(int i = 0; i < rows; ++i)
            for(int j = 0; j < m.cols; ++j)
            {
                res.matrix[i][j] = 0;
                for(int n = 0; n < cols; ++n)
                {
                    res.matrix[i][j] += matrix[i][n] * m.matrix[n][j];
                }
            }

        return res;
    }

    void print()
    {
        for(long[] row : matrix)
        {
            for(long elem : row)
            {
                System.out.print(elem + " ");
            }
            System.out.print("\n");
        }
    }

    long sumMinInColumns()
    {
        long sum = 0;
        for(int i = 0; i < rows; ++i)
        {
            long t = matrix[0][i];
            for(int j = 0; j < cols; ++j)
            {
                if(matrix[j][i] < t)
                    t = matrix[j][i];
            }
            sum += t;
        }
        return sum;
    }
}

public class Lab2
{
    public static final int VARIANT = 5324;
    public static final int C5 = VARIANT % 5;
    public static final int C7 = VARIANT % 7;
    public static final int C11 = VARIANT % 11;

    public static void main(String[] args)
    {
        System.out.println("Variant: " + VARIANT + ", C5: " + C5 + ", C7: " + C7 + ", C11: " + C11);
        Matrix a = new Matrix();
        a.fill();
        Matrix b = new Matrix();
        b.fill();

        Matrix c;
        try {
            System.out.println("С = A * B = ");
            c = a.mul(b);
            c.print();
            System.out.println("Sum of minimal elements in C's columns: " + c.sumMinInColumns());
        } catch (MatrixMultiplyException e) {
            System.out.println(e.getMessage());
        }
    }
}
*/
