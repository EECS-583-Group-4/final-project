#include <stdio.h>

struct RT {
  char A;
  int B[10][20];
  char C;
};

struct ST {
  int X;
  double Y;
  struct RT Z;
};

int *foo(struct ST *s) {
  return &s[1].Z.B[5][13];
}

struct munger_struct {
  int f1;
  int f2;
};

void munge(struct munger_struct *P) {
  P[0].f1 = P[1].f1 + P[2].f2;
}

int main()
{
  struct ST s[10];
  int *p = foo(s);

  struct munger_struct arr[3];

  munge(arr);

  return 0;
}