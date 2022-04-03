#include <stdio.h>
#include "lib.h"

int main()
{
    char mode;
    printf("To compute integral sin(x) [A, B] with accuracy = e enter -- 1 A B e\nTo compute figure area with sides lengths A, B enter -- 2 A B\n");
    while ((mode = getchar()) != EOF) {
        // compute integral
        if (mode == '\n') continue;
        if (mode == '1') {
            float A, B, e;
            scanf("%f%f%f", &A, &B, &e);
            printf("Integral: %.10f\n", SinIntegral(A, B, e));
        } else if (mode == '2') {
            float A, B;
            scanf("%f%f", &A, &B);
            printf("Area: %f\n", Square(A, B));
        } else {
            printf("This command is not supported, enter 1 or 0\n");
        }
    }
    return 0;
}