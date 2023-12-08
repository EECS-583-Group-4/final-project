// toy.c
#include <stdio.h>
#define SIZE 200


void never_executed(){
printf("Should Not Be Printed!\n");
}

int iAdd(int a, int b){
return a+b;
}

float fAdd(float a, float b){
return a+b;
}


int main(){
int A[SIZE] = {0};
int B[SIZE] = {1};
float C[SIZE] = {3.2};
float D[SIZE] = {0.1};

for(int i=0;i<SIZE;++i){
D[i] = fAdd(C[i], D[i+5]);

B[i] = iAdd(A[i*2], B[i]);
}

return 0;
}