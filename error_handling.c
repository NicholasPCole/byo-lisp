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

enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };
enum { LVAL_NUM, LVAL_ERR };

typedef struct {
    int type;
    long number;
    int error;
} lval;

// Constructs an lval using appropriate fields for a number type.
lval lval_num(long x) {
    lval v;
    v.type = LVAL_NUM;
    v.number = x;
    return v;
}

// Constructs an lval using appropriate fields for an error type.
lval lval_err(int x) {
    lval v;
    v.type = LVAL_ERR;
    v.error = x;
    return v;
}

// Prints an lval as appropriate for a number or error type.
void lval_print(lval v) {
    switch (v.type) {
        case LVAL_NUM:
            printf("%li", v.number);
            break;
        case LVAL_ERR:
            if (v.error == LERR_DIV_ZERO) {
                printf("Error: Divide By Zero!");
            } else if (v.error == LERR_BAD_OP) {
                printf("Error: Invalid Operator!");
            } else if (v.error == LERR_BAD_NUM) {
                printf("Error: Invalid Number!");
            }

            break;
    }
}

// Prints an lval followed by a newline.
void lval_println(lval v) {
    lval_print(v);
    putchar('\n');
}

lval eval_op(lval x, char* operator, lval y) {
    // If either value is an error, return it instead of computing.
    
    if (x.type == LVAL_ERR) {
        return x;
    } else if (y.type == LVAL_ERR) {
        return y;
    }

    // Otherwise, do math on the number values.

    if (strcmp(operator, "+") == 0) {
        return lval_num(x.number + y.number);
    } else if (strcmp(operator, "-") == 0) {
        return lval_num(x.number - y.number);
    } else if (strcmp(operator, "*") == 0) {
        return lval_num(x.number * y.number);
    } else if (strcmp(operator, "/") == 0) {
        if (y.number == 0) {
            return lval_err(LERR_DIV_ZERO);
        } else {
            return lval_num(x.number / y.number);
        }
    } else if (strcmp(operator, "%") == 0) {
        return lval_num(x.number % y.number);
    } else {
        return lval_err(LERR_BAD_OP);
    }
}

lval eval(mpc_ast_t* tree) {
    // If tagged as a number, return it directly. Else, expression.
    
    if (strstr(tree->tag, "number")) {
        // Check if there is some error in conversion.

        errno = 0;
        long x = strtol(tree->contents, NULL, 10);

        if (errno != ERANGE) {
            return lval_num(x);
        } else {
            return lval_err(LERR_BAD_NUM);
        }
    }

    char* operator = tree->children[1]->contents;
    lval x = eval(tree->children[2]);

    // Iterate the remaining children, combining using the operator.

    int i = 3;
    while (strstr(tree->children[i]->tag, "expr")) {
        x = eval_op(x, operator, eval(tree->children[i]));
        i++;
    }

    return x;
}

int main(int argc, char** argv) {
    // Create some parsers and define them with the following language.
    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* Lispy = mpc_new("lispy");
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
            lval result = eval(r.output);
            lval_println(result);
            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
    }

    // Undefine and delete our parsers. This point is never reached since there
    // is nothing in the language to escape the read-evaluate-print loop.
    mpc_cleanup(4, Number, Operator, Expr, Lispy);

    return 0;
}
