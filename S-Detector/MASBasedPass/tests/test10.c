#include <stdio.h>

#define SIZE 10000
#define OFFSET 0
#define INCREMENT 3

int main() {
    int a[SIZE] = {0};
    int b[SIZE*2] = {0};
    int c[SIZE/2] = {0};
    int d[SIZE] = {0};
    for (int i = OFFSET; i < SIZE + OFFSET; i+=INCREMENT){
        printf("%d \n", a[i-OFFSET]);
        printf("%d \n", a[(i-OFFSET)*2]);
        printf("%d \n", a[(i-OFFSET)/2]);
        printf("%d \n", a[(i-OFFSET)]);
    }
    return 0;
}