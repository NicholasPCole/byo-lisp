#include <stdio.h>

void print_hello(int n) {
    for (int i = 1; i <= n; i++) {
        puts("Function Hello World!");
    }

    return;
}

int main(int argc, char** argv) {
    for (int i = 0; i < 5; i++) {
        puts("For Hello World!");
    }

    int i = 0;
    while (i < 5) {
        puts("While Hello World!");
        i++;
    }

    print_hello(3);

    return 0;
}
