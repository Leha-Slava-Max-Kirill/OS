#include <stdio.h>
#include <string.h>

int count(char *str)
{
    int counter = 1;
    for (int i = 0; i < strlen(str); ++i)
        if (str[i] == ' ')
            counter++;
    return counter;
}

float sum(FILE *f, int count)
{
    float res = 0.0, a;
    rewind(f);
    for (int i = 0; i < count; ++i) {
        fscanf(f, "%f", &a);
        res += a;
    }
    return res;
}