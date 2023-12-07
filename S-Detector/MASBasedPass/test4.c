#include <stdio.h>
#include <stdlib.h>

// Code snippet from Figure 9 of Spindle
// TODO: Change this code to replicate memory bugs

// Define cost_t type
typedef int cost_t;

struct test_struct {
    int* head;
    int* tail;
    cost_t flow;
};

void test_func() {
    // Allocate memory for new array
    struct test_struct new[100];
    int pos = 1;
    int red_cost = 0;
    int* tail = new[pos-1].tail;
    
    while (pos -1 && red_cost > (cost_t)new[pos/2-1].flow) {
        new[pos-1].tail = new[pos/2-1].tail;
        new[pos-1].head = new[pos/2-1].head;
        // Three more acccesses to struct members
        // of new[pos-1] and new[pos/2-1].
        pos = pos/2;
        new[pos-1].tail = tail;
        // Four more accesses to struct members
        // of new[pos-1].
    }

    return;
}


int main() {
    test_func();
    return 0;
}