#include <stdio.h>
#define SIZE 110000

void printArray(int arr[])
{
    int i;
    for (i = 0; i < SIZE; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

int main()
{
    int arr[SIZE] = {0};
    for (int i = 0; i < SIZE; ++i)
    {
        arr[SIZE - 1 - i] = i;
    }
    printArray(arr);
    int target = 1;

    int left = 0;
    int right = SIZE - 1;

    for (int i = 0; i < SIZE - 1; ++i)
    {
        for (int j = 0; j < SIZE - i - 1; ++j)
        {
            if (arr[j] > arr[j + 1])
            {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }

    printArray(arr);
    return 0;
}
