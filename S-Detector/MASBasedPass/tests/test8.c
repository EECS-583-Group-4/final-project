#include <stdio.h>
#include <stdlib.h>

// Test correct memory allocation and deallcoation

int main() {
    int *a = malloc(sizeof(int));
    *a = 5;
    printf("%d \n", *a);
    free(a);
    return 0;
}