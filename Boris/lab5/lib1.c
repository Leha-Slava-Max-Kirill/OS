#include "lib.h"
#include <math.h>
#include <stdio.h>

long long facto(long long f)
{
    return (f == 0) ? 1: (f * facto(f - 1));
}
// coumpute Pi with Leibniz Series
double ComputePi(int k) 
{
    long double res = 0;
    long double flag = 0;
    for (long long i = 0; i <= k; i++) {
        flag = (i & 1) ? -1: 1;
        res += flag / (long long)(2 * i + 1);
    }
    res *= 4;
    return res;
}
// compute e with taylor series
double ComputeE(int x) {
    x = (x > 20) ? 20: x;
    double res = 0;
    for (int i = 0; i <= x; i++) {
        res += (double)1/facto(i);
    }
    return res;
}
