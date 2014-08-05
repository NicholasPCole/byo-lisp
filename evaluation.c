#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

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

long eval_op(long x, char* operator, long y) {
    if (strcmp(operator, "+") == 0) {
        return x + y;
    } else if (strcmp(operator, "-") == 0) {
        return x - y;
    } else if (strcmp(operator, "*") == 0) {
        return x * y;
    } else if (strcmp(operator, "/") == 0) {
        return x / y;
    } else if (strcmp(operator, "%") == 0) {
        return x % y;
    } else {
        return 0;
    }
}

long eval(mpc_ast_t* tree) {
    // If tagged as a number, return it directly. Else, expression.
    
    if (strstr(tree->tag, "number")) {
        return atoi(tree->contents);
    }

    // The operator is always the second child (after an opening parenthesis).
    
    char* operator = tree->children[1]->contents;

    // Store the third child in x.

    long x = eval(tree->children[2]);

    // Iterate the remaining children, combining using the operator.

    int i = 3;
    while (strstr(tree->children[i]->tag, "expr")) {
        x = eval_op(x, operator, eval(tree->children[i]));
        i++;
    }

    return x;
}

int main(int argc, char** argv) {
    // Create some parsers.

    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* Lispy = mpc_new("lispy");

    // Define them with the following language.

    mpca_lang(MPCA_LANG_DEFAULT,
            "                                                  \
            number   : /-?[0-9]+/ ;                            \
            operator : '+' | '-' | '*' | '/' | '%' ;           \
            expr     : <number> | '(' <operator> <expr>+ ')' ; \
            lispy    : /^/ <operator> <expr>+ /$/ ;            \
            ", Number, Operator, Expr, Lispy);

    puts("byo-lisp Version 0.0.1");
    puts("Author: Nicholas P. Cole");
    puts("Press Ctrl-C to exit.");

    while (1) {
        char* input = readline("byo-lisp> ");
        add_history(input);

        // Attempt to parse the user input.
        
        mpc_result_t r;
        
        if (mpc_parse("<stdin>", input, Lispy, &r)) {
            long result = eval(r.output);
            printf("%li\n", result);
            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
    }

    // Undefine and delete our parsers.

    mpc_cleanup(4, Number, Operator, Expr, Lispy);

    return 0;
}
