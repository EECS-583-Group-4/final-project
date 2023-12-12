#include <stdio.h>

#define SIZE 1000
#define OFFSET 7
#define INCREMENT 3

int main()
{
    int a[SIZE] = {0};
    int b[SIZE * 2] = {0};

    for (int i = OFFSET; i < SIZE + OFFSET; i *= INCREMENT)
    {
        printf("%d \n", a[(i - OFFSET) * 2]);
    }
    return 0;
}