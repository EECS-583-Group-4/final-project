#include <stdio.h>

// test_loop_out_of_bound.c from Spindle

int main() {
    int a[1000];
    char str[20];
    for (int i = 0; i < 1001; ++i) {
        sprintf(str, "%d", a[i]);
    }
    return 0;
}