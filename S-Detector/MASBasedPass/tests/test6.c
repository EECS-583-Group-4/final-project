#include <stdio.h>
#include <stdlib.h>

// Tests use after free

int main() {
    int *a = malloc(sizeof(int));
    free(a);
    int val = *a;
    printf("%d \n", *a);
    return 0;
}