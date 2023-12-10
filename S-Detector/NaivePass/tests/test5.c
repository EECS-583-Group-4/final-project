#include <stdio.h>

// test_loop_out_of_bound.c from Spindle

int main() {
    int a[10] = {0};
    for (int i = 400; i > 10; i/=2) {
        printf("%d \n", a[i]);
    }
    return 0;
}