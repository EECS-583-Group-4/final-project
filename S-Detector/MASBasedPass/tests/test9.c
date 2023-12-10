#include <stdio.h>
#include <stdlib.h>

// Tests deallocating an unallocated object

int main() {
    int *a = NULL;
    free(a);
    return 0;
}