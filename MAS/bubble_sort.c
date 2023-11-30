#include "stdlib.h"

void Swap(int *, int, int);

void BubbleSort(int *A, int N)
{
	for (int i = 0; i < N; ++i)
	{
		for (int j = i + 1; j < N; ++j)
		{
			_Bool flag = (A[i] > A[j]);
			if (flag)
			{
				Swap(A, i, j);
			}
		}
	}
}

void Swap(int *S, int i, int j)
{
	int tmp = S[i];
	S[i] = S[j];
	S[j] = tmp;
}

int main() {

	int a[] = {5, 2, 3, 4, 1};
	int b = 5;

	BubbleSort(a, b);

	return 0;
}
