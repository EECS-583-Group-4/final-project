#include <stdio.h>

// test_loop_out_of_bound.c from Spindle

int main()
{
    int a[10] = {0};
    for (int i = 0; i < 10; ++i)
    {
        printf("%d \n", a[i]);
    }
    return 0;
}