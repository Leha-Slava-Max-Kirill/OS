#include <stdio.h>
#include "lib.h"

int main()
{
    char mode;
    printf("To calculate the number pi with the long series k enter -- 1\n");
    printf("To calculate the number e enter -- 2 \n");
    while ((mode = getchar()) != EOF) {
        if (mode == '\n') continue;
        if (mode == '1') {
            int k;
            printf("Enter the series lenght k: ");
            scanf("%d", &k);
            printf("The value of the counted number pi: %.9lf\n", ComputePi(k));
        } else if (mode == '2') {
            int x;
            printf("Enter the number: ");
            scanf("%d", &x);
            printf("The value of the counted number e: %lf\n", ComputeE(x));
        } else {
            printf("This command is not supported, enter 1 or 0\n");
            // long long n;
            // scanf("%lld", &n);
            // printf("factorial %lld = %lld\n", n, facto(n)); 
        }
    }
    // int i = 20000000;
    // ComputePi(i);
    return 0;
}