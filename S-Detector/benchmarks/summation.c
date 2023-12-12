#include <stdio.h>

#define SIZE 70000

int main()
{
    int a[SIZE] = {1};
    int b[SIZE] = {1};
    int sum[SIZE] = {0};

    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            a[j] = a[i] + b[i] + a[j] + b[j];
            b[j] = a[i] + b[i] + a[j] + b[j];
            sum[i] = a[i] + b[i] + a[j] + b[j];
        }
    }
    return 0;
}