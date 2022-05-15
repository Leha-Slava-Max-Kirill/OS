#include "lib.h"
#include <stdio.h>
#include <math.h>

#define M_PI 3.14159265358979323846

//compute Pi with Wallis formula
double ComputePi(int k)
{
    k = (k > 44749) ? 44749: k;
    long double res = 1;
    for (int i = 1; i <= k; i++) {
        if( i > 23170) res *= 1.0000000005;
        else res *= (long double)(4 * i * i) / (4 * i * i - 1);
    }
    res *= 2;
    return (double)res;
}
// compute e with (1 + 1 / x) ^ x
double ComputeE(int x)
{
    x = (x > 0) ? x: x * -1;
    return pow((1 + (double)1 / x), x);
}
