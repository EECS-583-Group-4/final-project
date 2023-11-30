#include <stdio.h>

struct Test {
    int a[10];
} typedef Test;

struct Other {
    int b;
} typedef Other;

int main() {

    Test t;
    Other o;


    o.b = 0;
    t.a[o.b] = 0;

    for (int i = 0; i < 11; i++) {
        o.b = i;
        t.a[1] = t.a[0] + t.a[o.b];
    }  

    return 0;
}