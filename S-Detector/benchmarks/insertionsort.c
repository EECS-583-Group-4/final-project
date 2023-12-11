#include <stdio.h>
#define SIZE 100000

void printArray(int arr[]) {
    int i;
    for (i = 0; i < SIZE; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

int main() {
    int arr[SIZE] = {0};
    for (int i = 0; i < SIZE; ++i){
        arr[SIZE - 1 - i] = i;
    } 
    printArray(arr);
    int i, key, j;

    for (i = 1; i < SIZE; i++) {
        key = arr[i];
        j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
    printArray(arr);

    return 0;
}


