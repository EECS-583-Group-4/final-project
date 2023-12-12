#include <stdio.h>

#define SIZE 1000

int main()
{
    int a[SIZE] = {0};
    int b[SIZE * 2] = {0};

    // i ranges from 7 to 567
    for (int i = 7; i < SIZE + 7; i *= 3)
    {
        // array index ranges from 0 to 1120
        printf("%d \n", a[(i - 7) * 2]);
    }
    return 0;
}