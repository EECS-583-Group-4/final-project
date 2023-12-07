// toy.c
#include <stdio.h>
#define SIZE 200


void never_executed(){
printf("Should Not Be Printed!\n");
}

int main(){
int A[SIZE] = {0};
int B[SIZE] = {1};
float C[SIZE] = {3.2};
float D[SIZE] = {0.1};

for(int i=0;i<SIZE;i++){
D[i] = C[i] + D[i];
}

return 0;
}