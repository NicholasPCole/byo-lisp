#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

#ifdef _WIN32

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

enum { LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR };

typedef struct lval {
    int type;
    long number;

    // Error and Symbol types have some string data.
    char* error;
    char* symbol;

    // Count and pointer to a list of lval.
    int count;
    struct lval** cell;
} lval;

// Constructs a pointer to a new lval using appropriate fields for a Number lval.
lval* lval_num(long x) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->number = x;
    return v;
}

// Constructs a pointer to a new lval using appropriate fields for an Error lval.
lval* lval_err(char* m) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_ERR;
    v->error = malloc(strlen(m) + 1);
    strcpy(v->error, m);
    return v;
}

// Constructs a pointer to a new lval using appropriate fields for a Symbol lval.
lval* lval_sym(char* s) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SYM;
    v->symbol = malloc(strlen(s) + 1);
    strcpy(v->symbol, s);
    return v;
}

// Constructs a pointer to a new lval using appropriate fields for an S-expression lval.
lval* lval_sexpr(void) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

// Constructs a pointer to a new lval using appropriate fields for a Q-expression lval.
lval* lval_qexpr(void) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_QEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

void lval_del(lval* v) {
    switch (v->type) {
        case LVAL_NUM:
            // Do not do anything special for a number type.
            break;
        case LVAL_ERR:
            // Free the relevant string data.
            free(v->error);
            break;
        case LVAL_SYM:
            // Free the relevant string data.
            free(v->symbol);
            break;
        case LVAL_SEXPR:
        case LVAL_QEXPR:
            // Delete all the elements inside.
            for (int i = 0; i < v->count; i++) {
                lval_del(v->cell[i]);
            }

            // Also free the memory allocated to contain the pointers.
            free(v->cell);

            break;
    }

    // Finally, free the memory allocated for the lval struct itself.
    free(v);
}

lval* lval_add(lval* v, lval* x) {
    v->count++;
    v->cell = realloc(v->cell, sizeof(lval*) * v->count);
    v->cell[v->count - 1] = x;
    return v;
}

lval* lval_pop(lval* v, int i) {
    // Find the item at i.
    lval* x = v->cell[i];

    // Shift the memory following the item at i over the top of it.
    memmove(&v->cell[i], &v->cell[i + 1], sizeof(lval*) * (v->count - i - 1));

    // Decrease the count of items in the list.
    v->count--;

    // Reallocate the memory used.
    v->cell = realloc(v->cell, sizeof(lval*) * v->count);

    return x;
}

lval* lval_join(lval* x, lval* y) {
    while (y->count) {
        x = lval_add(x, lval_pop(y, 0));
    }

    lval_del(y);
    return x;
}

lval* lval_take(lval* v, int i) {
    lval* x = lval_pop(v, i);
    lval_del(v);
    return x;
}

void lval_print(lval* v);

void lval_expr_print(lval* v, char open, char close) {
    putchar(open);

    for (int i = 0; i < v->count; i++) {
        lval_print(v->cell[i]);

        // Don't print the trailing space if this is the last element.

        if (i != (v->count - 1)) {
            putchar(' ');
        }
    }

    putchar(close);
}

// Prints an lval as appropriate for a number or error type.
void lval_print(lval* v) {
    switch (v->type) {
        case LVAL_NUM:
            printf("%li", v->number);
            break;
        case LVAL_ERR:
            printf("Error: %s", v->error);
            break;
        case LVAL_SYM:
            printf("%s", v->symbol);
            break;
        case LVAL_SEXPR:
            lval_expr_print(v, '(', ')');
            break;
        case LVAL_QEXPR:
            lval_expr_print(v, '{', '}');
            break;
    }
}

// Prints an lval followed by a newline.
void lval_println(lval* v) {
    lval_print(v);
    putchar('\n');
}

lval* builtin_op(lval* a, char* op) {
    // Ensure all arguments are numbers.

    for (int i = 0; i < a->count; i++) {
        if (a->cell[i]->type != LVAL_NUM) {
            lval_del(a);
            return lval_err("Cannot operate on non-number!");
        }
    }

    // Pop the first element.

    lval* x = lval_pop(a, 0);

    // If no arguments and sub, then perform unary negation.

    if ((strcmp(op, "-") == 0) && (a->count == 0)) {
        x->number = -x->number;
    }

    // While there are still elements remaining...

    while (a->count > 0) {
        // Pop the next element.

        lval* y = lval_pop(a, 0);

        // Perform the operation.

        if (strcmp(op, "+") == 0) {
            x->number += y->number;
        } else if (strcmp(op, "-") == 0) {
            x->number -= y->number;
        } else if (strcmp(op, "*") == 0) {
            x->number *= y->number;
        } else if (strcmp(op, "/") == 0) {
            if (y->number == 0) {
                lval_del(x);
                lval_del(y);
                x = lval_err("Division By Zero");
                break;
            }

            x->number /= y->number;
        }

        // Delete the element, now that we're finished with it.

        lval_del(y);
    }

    // Delete the input expression and return the result.

    lval_del(a);
    return x;
}

#define LASSERT(args, cond, err) if (!(cond)) { lval_del(args); return lval_err(err); }

lval* lval_eval(lval* v);

lval* builtin_list(lval* a) {
    a->type = LVAL_QEXPR;
    return a;
}

lval* builtin_head(lval* a) {
    LASSERT(a, (a->count == 1), "Function 'head' passed too many arguments.");
    LASSERT(a, (a->cell[0]->type == LVAL_QEXPR), "Function 'head' passed incorrect type.");
    LASSERT(a, (a->cell[0]->count != 0), "Function 'head' passed {}.");

    lval* v = lval_take(a, 0);

    while (v->count > 1) {
        lval_del(lval_pop(v, 1));
    }

    return v;
}

lval* builtin_tail(lval* a) {
    LASSERT(a, (a->count == 1), "Function 'tail' passed too many arguments.");
    LASSERT(a, (a->cell[0]->type == LVAL_QEXPR), "Function 'tail' passed incorrect type.");
    LASSERT(a, (a->cell[0]->count != 0), "Function 'tail' passed {}.");

    lval* v = lval_take(a, 0);
    lval_del(lval_pop(v, 0));
    return v;
}

lval* builtin_eval(lval* a) {
    LASSERT(a, (a->count == 1), "Function 'eval' passed too many arguments.");
    LASSERT(a, (a->cell[0]->type == LVAL_QEXPR), "Function 'eval' passed incorrect type.");

    lval* x = lval_take(a, 0);
    x->type = LVAL_SEXPR;
    return lval_eval(x);
}

lval* builtin_join(lval* a) {
    for (int i = 0; i < a->count; i++) {
        LASSERT(a, (a->cell[i]->type == LVAL_QEXPR), "Function 'join' passed incorrect type.");
    }

    lval* x = lval_pop(a, 0);

    while (a->count) {
        x = lval_join(x, lval_pop(a, 0));
    }

    lval_del(a);
    return x;
}

lval* builtin(lval*a, char* func) {
    if (strcmp("list", func) == 0) {
        return builtin_list(a);
    } else if (strcmp("head", func) == 0) {
        return builtin_head(a);
    } else if (strcmp("tail", func) == 0) {
        return builtin_tail(a);
    } else if (strcmp("join", func) == 0) {
        return builtin_join(a);
    } else if (strcmp("eval", func) == 0) {
        return builtin_eval(a);
    } else if (strstr("+-*/", func)) {
        return builtin_op(a, func);
    }

    lval_del(a);
    return lval_err("Unknown function!");
}

lval* lval_eval_sexpr(lval* v) {
    // Evaluate the children.

    for (int i = 0; i < v->count; i++) {
        v->cell[i] = lval_eval(v->cell[i]);
    }

    // Perform error checking.

    for (int i = 0; i < v->count; i++) {
        if (v->cell[i]->type == LVAL_ERR) {
            return lval_take(v, i);
        }
    }

    // Is the expression empty?

    if (v->count == 0) {
        return v;
    }

    // Does the expression have only one element?

    if (v->count == 1) {
        return lval_take(v, 0);
    }

    // Ensure the first element is a symbol.

    lval* f = lval_pop(v, 0);

    if (f->type != LVAL_SYM) {
        lval_del(f);
        lval_del(v);
        return lval_err("S-expression does not start with a symbol.");
    }

    // Call built-in with operator.

    lval* result = builtin(v, f->symbol);
    lval_del(f);
    return result;
}

lval* lval_eval(lval* v) {
    if (v->type == LVAL_SEXPR) {
        // Evaluate S-expressions.

        return lval_eval_sexpr(v);
    } else {
        // All other lval types remain the same.

        return v;
    }
}

lval* lval_read_num(mpc_ast_t* t) {
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    if (errno != ERANGE) {
        return lval_num(x);
    } else {
        return lval_err("Invalid number.");
    }
}

lval* lval_read(mpc_ast_t* t) {
    // If Symbol or Number, return the conversion to that type.

    if (strstr(t->tag, "number")) {
        return lval_read_num(t);
    } else if (strstr(t->tag, "symbol")) {
        return lval_sym(t->contents);
    }

    // If a root, S-expression, or Q-expression, then create an empty list.

    lval* x = NULL;

    if (strcmp(t->tag, ">") == 0) {
        x = lval_sexpr();
    } else if (strstr(t->tag, "sexpr")) {
        x = lval_sexpr();
    } else if (strstr(t->tag, "qexpr")) {
        x = lval_qexpr();
    }

    // Then fill this list with any valid expression contained within.

    for (int i = 0; i < t->children_num; i++) {
        if (strcmp(t->children[i]->contents, "(") == 0) {
            continue;
        } else if (strcmp(t->children[i]->contents, ")") == 0) {
            continue;
        } else if (strcmp(t->children[i]->contents, "{") == 0) {
            continue;
        } else if (strcmp(t->children[i]->contents, "}") == 0) {
            continue;
        } else if (strcmp(t->children[i]->tag, "regex") == 0) {
            continue;
        }

        x = lval_add(x, lval_read(t->children[i]));
    }

    return x;
}

int main(int argc, char** argv) {
    // Create some parsers and define them with the following language.
    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Symbol = mpc_new("symbol");
    mpc_parser_t* Sexpr = mpc_new("sexpr");
    mpc_parser_t* Qexpr = mpc_new("qexpr");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* Lispy = mpc_new("lispy");
    mpca_lang(MPCA_LANG_DEFAULT,
            "                                                  \
            number   : /-?[0-9]+/ ;                            \
            symbol   : \"list\" | \"head\" | \"tail\" | \"eval\" | \"join\" | '+' | '-' | '*' | '/' | '%' ;           \
            sexpr    : '(' <expr>* ')' ;                       \
            qexpr    : '{' <expr>* '}' ;                       \
            expr     : <number> | <symbol> | <sexpr> | <qexpr> ;        \
            lispy    : /^/ <expr>* /$/ ;                       \
            ", Number, Symbol, Sexpr, Qexpr, Expr, Lispy);

    puts("byo-lisp Version 0.0.1");
    puts("Author: Nicholas P. Cole");
    puts("Press Ctrl-C to exit.");

    while (1) {
        char* input = readline("byo-lisp> ");
        add_history(input);

        // Attempt to parse the user input.
        
        mpc_result_t r;
        
        if (mpc_parse("<stdin>", input, Lispy, &r)) {
            lval* x = lval_eval(lval_read(r.output));
            lval_println(x);
            lval_del(x);
            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
    }

    // Undefine and delete our parsers. This point is never reached since there
    // is nothing in the language to escape the read-evaluate-print loop.
    mpc_cleanup(6, Number, Symbol, Sexpr, Qexpr, Expr, Lispy);

    return 0;
}