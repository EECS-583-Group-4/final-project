#include <stdio.h>

#define SIZE 7000
#define OFFSET 6
#define INCREMENT 7

int main()
{
    int a[SIZE] = {0};
    int b[SIZE * 2] = {0};
    int c[SIZE / 2] = {0};
    int d[SIZE] = {0};
    for (int i = OFFSET; i < SIZE + OFFSET; i += INCREMENT)
    {
        for (int j = OFFSET; j < SIZE + OFFSET; j += INCREMENT)
        {
            for (int k = OFFSET; k < SIZE + OFFSET; k += INCREMENT)
            {
                a[i - OFFSET] = a[i - OFFSET] + 1;
                b[(i - OFFSET) * 2] = b[(i - OFFSET) * 2] + 1;
                c[(i - OFFSET) / 2] = c[(i - OFFSET) / 2] + 1;
                d[i - OFFSET] = d[i - OFFSET] + 1;
                a[j - OFFSET] = a[j - OFFSET] + 1;
                b[(j - OFFSET) * 2] = b[(j - OFFSET) * 2] + 1;
                c[(j - OFFSET) / 2] = c[(j - OFFSET) / 2] + 1;
                d[j - OFFSET] = d[j - OFFSET] + 1;
                a[k - OFFSET] = a[k - OFFSET] + 1;
                b[(k - OFFSET) * 2] = b[(k - OFFSET) * 2] + 1;
                c[(k - OFFSET) / 2] = c[(k - OFFSET) / 2] + 1;
                d[k - OFFSET] = d[k - OFFSET] + 1;
            }
        }
    }
    return 0;
}