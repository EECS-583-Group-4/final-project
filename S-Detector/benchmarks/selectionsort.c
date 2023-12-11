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

        int i, j, min_idx;
    for (i = SIZE-1; i >= 0; i--) {
        min_idx = i;
        for (j = SIZE - 1; j > i; j--) {
            if (arr[j] < arr[min_idx]){
                min_idx = j;
            }
        }

        // Swap the found minimum element with the first element
        int temp = arr[min_idx];
                arr[min_idx] = arr[i];
                arr[i] = temp;
    }
    printArray(arr);

    return 0;
}


