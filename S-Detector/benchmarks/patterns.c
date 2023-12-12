#include <stdio.h>

#define SIZE 800

int main()
{
    int a[SIZE][SIZE];
    int b[SIZE][SIZE];
    int c[SIZE][SIZE];
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            a[i][j] = i * j;
            b[i][j] = i * j;
            c[i][j] = i * j;
        }
    }
    for (int rep = 0; rep < 2; ++rep)
    {
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < SIZE; j++)
            {
                for (int k = 0; k < SIZE; k++)
                {
                    a[i][j] = (c[k][j] + 1) * b[j][k] * b[i][k] * a[j][k];
                    b[i][j] = (a[k][j] + 2) * c[j][k] * b[i][k] * a[j][k];
                    c[i][j] = (b[k][j] + 3) * a[j][k] * b[i][k] * a[j][k];
                }
            }
        }
    }
    return 0;
}