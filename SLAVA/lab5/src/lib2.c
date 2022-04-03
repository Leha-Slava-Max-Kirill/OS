#include "lib.h"
#include <stdio.h>
#include <math.h>

float SinIntegralTrapeziod(float A, float B, float e)
{
    int n = (B - A) / e;
    float sum = 0.5 * (sin(A) + sin(B));
    for (int i = 1; i < n; ++i)
        sum += sin(A + i * e);
    return sum * e;
}

float Square(float A, float B)
{
    if (A < 0) {
        printf("Enter A > 0\n");
        return -1;
    }
    if (B < 0) {
        printf("Enter B > 0\n");
        return -1;
    }
    return A * B;
}

/*
1) Рассчет интеграла функции sin(x) на отрезке [A, B] с шагом e
Float SinIntegral(float A, float B, float e)
Подсчет интеграла методом трапеций.

2) Подсчет площади плоской геометрической фигуры по двум сторонам
Float Square(float A, float B)
Фигура прямоугольный треугольник
*/