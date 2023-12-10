#include <stdio.h>
#include <stdlib.h>

// Tests memory leak (dynamically allocated object remains unfreed upon program termination)

int main() {
    int *a = malloc(sizeof(int));
    return 0;
} 