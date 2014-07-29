#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32

#include <string.h>
// If compiling on Windows, define the following (fake) functions.

static char buffer[2048];

char* readline(char* prompt) {
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char* cpy = malloc(strlen(buffer) + 1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy) - 1] = '\0';
    return cpy;
}

void add_history(char* unused) {}

#else
// Else (on Linux or Mac), simply use the editline library.

#include <editline/readline.h>
#include <editline/history.h>

#endif

// Declaring the input buffer static means that it is local to this file.
// static char input[2048];

int main(int argc, char** argv) {
    puts("byo-lisp Version 0.0.1");
    puts("Author: Nicholas P. Cole");
    puts("Press Ctrl-C to exit.");

    while (1) {
        char* input = readline("byo-lisp> ");
        add_history(input);
        printf("No, you're a %s.\n", input);
        free(input);
    }

    return 0;
}
