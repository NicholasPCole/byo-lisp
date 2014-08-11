#include "mpc.h"

#ifdef _WIN32
// If compiling on Windows, define the following (fake) functions.

// Declaring the input buffer static means that it is local to this file.
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

// Forward Declarations

struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;

// Lisp Value

enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_FUN, LVAL_SEXPR, LVAL_QEXPR };

typedef lval*(*lbuiltin)(lenv*, lval*);

struct lval {
    int type;

    long number;
    char* error;
    char* symbol;
    lbuiltin function;

    int count;
    lval** cell;
};

lval* lval_num(long x) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->number = x;
    return v;
}

lval* lval_err(char* fmt, ...) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_ERR;

    // Create a va list and initialize it.
    
    va_list va;
    va_start(va, fmt);

    // Allocate 512 B of space.

    v->error = malloc(512);

    // printf into the error string with a maximum of 511 characters.

    vsnprintf(v->error, 511, fmt, va);

    // Reallocate the number of bytes actually used.

    v->error = realloc(v->error, strlen(v->error) + 1);

    // Cleanup the va list.

    va_end(va);

    return v;
}

lval* lval_sym(char* s) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SYM;
    v->symbol = malloc(strlen(s) + 1);
    strcpy(v->symbol, s);
    return v;
}

lval* lval_fun(lbuiltin func) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_FUN;
    v->function = func;
    return v;
}

lval* lval_sexpr(void) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_SEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

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
        case LVAL_FUN:
            break;
        case LVAL_ERR:
            free(v->error);
            break;
        case LVAL_SYM:
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

lval* lval_copy(lval* v) {
    lval* x = malloc(sizeof(lval));
    x->type = v->type; // Found it!

    switch (v->type) {
        case LVAL_FUN:
            x->function = v->function;
            break;
        case LVAL_NUM:
            x->number = v->number;
            break;
        case LVAL_ERR:
            x->error = malloc(strlen(v->error) + 1);
            strcpy(x->error, v->error);
            break;
        case LVAL_SYM:
            x->symbol = malloc(strlen(v->symbol) + 1);
            strcpy(x->symbol, v->symbol);
            break;
        case LVAL_SEXPR:
        case LVAL_QEXPR:
            x->count = v->count;
            x->cell = malloc(sizeof(lval*) * x->count);

            for (int i = 0; i < x->count; i++) {
                x->cell[i] = lval_copy(v->cell[i]);
            }

            break;
    }

    return x;
}

lval* lval_add(lval* v, lval* x) {
    v->count++;
    v->cell = realloc(v->cell, sizeof(lval*) * v->count);
    v->cell[v->count - 1] = x;
    return v;
}

lval* lval_join(lval* x, lval* y) {
    for (int i = 0; i < y->count; i++) {
        x = lval_add(x, y->cell[i]);
    }

    free(y->cell);
    free(y);
    return x;
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

lval* lval_take(lval* v, int i) {
    lval* x = lval_pop(v, i);
    lval_del(v);
    return x;
}

void lval_print(lval* v);

void lval_print_expr(lval* v, char open, char close) {
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

void lval_print(lval* v) {
    switch (v->type) {
        case LVAL_FUN:
            printf("<function>");
            break;
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
            lval_print_expr(v, '(', ')');
            break;
        case LVAL_QEXPR:
            lval_print_expr(v, '{', '}');
            break;
    }
}

void lval_println(lval* v) {
    lval_print(v);
    putchar('\n');
}

char* ltype_name(int t) {
    switch (t) {
        case LVAL_FUN:
            return "Function";
        case LVAL_NUM:
            return "Number";
        case LVAL_ERR:
            return "Error";
        case LVAL_SYM:
            return "Symbol";
        case LVAL_SEXPR:
            return "S-Expression";
        case LVAL_QEXPR:
            return "Q-Expression";
        default:
            return "Unknown";
    }
}

// Lisp Environment

struct lenv {
    int count;
    char** symbols;
    lval** values;
};

lenv* lenv_new(void) {
    lenv* e = malloc(sizeof(lenv));
    e->count = 0;
    e->symbols = NULL;
    e->values = NULL;
    return e;
}

void lenv_del(lenv* e) {
    for (int i = 0; i < e->count; i++) {
        free(e->symbols[i]);
        lval_del(e->values[i]);
    }

    free(e->symbols);
    free(e->values);
    free(e);
}

lval* lenv_get(lenv* e, lval* k) {
    for (int i = 0; i < e->count; i++) {
        if (strcmp(e->symbols[i], k->symbol) == 0) {
            return lval_copy(e->values[i]);
        }
    }

    return lval_err("Unbound symbol '%s'", k->symbol);
}

void lenv_put(lenv* e, lval* k, lval* v) {
    // Iterate over all items in the environment to see if the variable already exists.

    for (int i = 0; i < e->count; i++) {
        if (strcmp(e->symbols[i], k->symbol) == 0) {
            lval_del(e->values[i]);
            e->values[i] = lval_copy(v);
            return;
        }
    }

    // If no existing entry is found, then allocate space for the new entry.

    e->count++;
    e->values = realloc(e->values, sizeof(lval*) * e->count);
    e->symbols = realloc(e->symbols, sizeof(char*) * e->count);

    // Copy the contents of lval and symbol string into new location.

    e->values[e->count - 1] = lval_copy(v);
    e->symbols[e->count - 1] = malloc(strlen(k->symbol) + 1);
    strcpy(e->symbols[e->count - 1], k->symbol);
}

// Builtins

#define LASSERT(args, cond, fmt, ...) \
  if (!(cond)) { lval* err = lval_err(fmt, ##__VA_ARGS__); lval_del(args); return err; }

#define LASSERT_TYPE(func, args, index, expect) \
  LASSERT(args, args->cell[index]->type == expect, \
    "Function '%s' passed incorrect type for argument %i. Got %s, Expected %s.", \
    func, index, ltype_name(args->cell[index]->type), ltype_name(expect))

#define LASSERT_NUM(func, args, num) \
  LASSERT(args, args->count == num, \
    "Function '%s' passed incorrect number of arguments. Got %i, Expected %i.", \
    func, args->count, num)

#define LASSERT_NOT_EMPTY(func, args, index) \
  LASSERT(args, args->cell[index]->count != 0, \
    "Function '%s' passed {} for argument %i.", func, index);

lval* lval_eval(lenv* e, lval* v);

lval* builtin_list(lenv* e, lval* a) {
    a->type = LVAL_QEXPR;
    return a;
}

lval* builtin_head(lenv* e, lval* a) {
    LASSERT_NUM("head", a, 1);
    LASSERT_TYPE("head", a, 0, LVAL_QEXPR);
    LASSERT_NOT_EMPTY("head", a, 0);

    lval* v = lval_take(a, 0);

    while (v->count > 1) {
        lval_del(lval_pop(v, 1));
    }

    return v;
}

lval* builtin_tail(lenv* e, lval* a) {
    LASSERT_NUM("tail", a, 1);
    LASSERT_TYPE("tail", a, 0, LVAL_QEXPR);
    LASSERT_NOT_EMPTY("tail", a, 0);
    
    lval* v = lval_take(a, 0);
    lval_del(lval_pop(v, 0));
    return v;
}

lval* builtin_eval(lenv* e, lval* a) {
    LASSERT_NUM("eval", a, 1);
    LASSERT_TYPE("eval", a, 0, LVAL_QEXPR);

    lval* x = lval_take(a, 0);
    x->type = LVAL_SEXPR;
    return lval_eval(e, x);
}

lval* builtin_join(lenv* e, lval* a) {
    for (int i = 0; i < a->count; i++) {
        LASSERT_TYPE("join", a, i, LVAL_QEXPR);
    }

    lval* x = lval_pop(a, 0);

    while (a->count) {
        lval* y = lval_pop(a, 0);
        x = lval_join(x, y);
    }

    lval_del(a);
    return x;
}

lval* builtin_op(lenv* e, lval* a, char* op) {
    for (int i = 0; i < a->count; i++) {
        LASSERT_TYPE(op, a, i, LVAL_NUM);
    }

    lval* x = lval_pop(a, 0);

    if ((strcmp(op, "-") == 0) && (a->count == 0)) {
        x->number = -x->number;
    }

    while (a->count > 0) {
        lval* y = lval_pop(a, 0);

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
                x = lval_err("Division by Zero");
                break;
            }

            x->number /= y->number;
        }

        lval_del(y);
    }

    lval_del(a);
    return x;
}

lval* builtin_add(lenv* e, lval* a) {
    return builtin_op(e, a, "+");
}

lval* builtin_sub(lenv* e, lval* a) {
    return builtin_op(e, a, "-");
}

lval* builtin_mul(lenv* e, lval* a) {
    return builtin_op(e, a, "*");
}

lval* builtin_div(lenv* e, lval* a) {
    return builtin_op(e, a, "/");
}

lval* builtin_def(lenv* e, lval* a) {
    LASSERT_TYPE("def", a, 0, LVAL_QEXPR);

    // The first argument is a symbol list.

    lval* symbols = a->cell[0];

    // Ensure all elements of the first list are symbols.

    for (int i = 0; i < symbols->count; i++) {
        LASSERT(a, (symbols->cell[i]->type == LVAL_SYM),
                "Function 'def' cannot define non-symbol. Got %s, expected %s.",
                ltype_name(symbols->cell[i]->type), ltype_name(LVAL_SYM));
    }

    // Check the correct number of symbols and values.

    LASSERT(a, (symbols->count == a->count - 1),
            "Function 'def' passed too many arguments for symbols. Got %i, expected %i.",
            symbols->count, a->count - 1);

    // Assign copies of values to symbols.

    for (int i = 0; i < symbols->count; i++) {
        lenv_put(e, symbols->cell[i], a->cell[i + 1]);
    }

    lval_del(a);
    return lval_sexpr();
}

void lenv_add_builtin(lenv* e, char* name, lbuiltin func) {
    lval* k = lval_sym(name);
    lval* v = lval_fun(func);
    lenv_put(e, k, v);
    lval_del(k);
    lval_del(v);
}

void lenv_add_builtins(lenv* e) {
    // Variable Functions

    lenv_add_builtin(e, "def", builtin_def);

    // List Functions

    lenv_add_builtin(e, "list", builtin_list);
    lenv_add_builtin(e, "head", builtin_head);
    lenv_add_builtin(e, "tail", builtin_tail);
    lenv_add_builtin(e, "eval", builtin_eval);
    lenv_add_builtin(e, "join", builtin_join);

    // Mathematical Functions

    lenv_add_builtin(e, "+", builtin_add);
    lenv_add_builtin(e, "-", builtin_sub);
    lenv_add_builtin(e, "*", builtin_mul);
    lenv_add_builtin(e, "/", builtin_div);
}

// Evaluation

lval* lval_eval_sexpr(lenv* e, lval* v) {
    // Evaluate the children.

    for (int i = 0; i < v->count; i++) {
        v->cell[i] = lval_eval(e, v->cell[i]);
    }

    // Perform error checking.

    for (int i = 0; i < v->count; i++) {
        if (v->cell[i]->type == LVAL_ERR) {
            return lval_take(v, i);
        }
    }

    if (v->count == 0) {
        return v;
    } else if (v->count == 1) {
        return lval_take(v, 0);
    }

    // Ensure the first element is a function after evaluation.

    lval* f = lval_pop(v, 0);

    if (f->type != LVAL_FUN) {
        lval* error = lval_err(
                "S-Expression starts with incorrect type. Got %s, expected %s.",
                ltype_name(f->type), ltype_name(LVAL_FUN));
        lval_del(f);
        lval_del(v);
        return error;
    }

    // Call built-in with operator.

    lval* result = f->function(e, v);
    lval_del(f);
    return result;
}

lval* lval_eval(lenv* e, lval* v) {
    if (v->type == LVAL_SYM) {
        lval* x = lenv_get(e, v);
        lval_del(v);
        return x;
    } else if (v->type == LVAL_SEXPR) {
        return lval_eval_sexpr(e, v);
    } else {
        return v;
    }
}

// Reading

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

// Main

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
            symbol   : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;      \
            sexpr    : '(' <expr>* ')' ;                       \
            qexpr    : '{' <expr>* '}' ;                       \
            expr     : <number> | <symbol> | <sexpr> | <qexpr> ;        \
            lispy    : /^/ <expr>* /$/ ;                       \
            ", Number, Symbol, Sexpr, Qexpr, Expr, Lispy);

    puts("byo-lisp Version 0.0.11");
    puts("Author: Nicholas P. Cole");
    puts("Press Ctrl-C to exit.");

    lenv* e = lenv_new();
    lenv_add_builtins(e);

    while (1) {
        char* input = readline("byo-lisp> ");
        add_history(input);

        mpc_result_t r;
        
        if (mpc_parse("<stdin>", input, Lispy, &r)) {
            lval* x = lval_eval(e, lval_read(r.output));
            lval_println(x);
            lval_del(x);
            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        free(input);
    }

    lenv_del(e);

    // Undefine and delete our parsers. This point is never reached since there
    // is nothing in the language to escape the read-evaluate-print loop.
    mpc_cleanup(6, Number, Symbol, Sexpr, Qexpr, Expr, Lispy);

    return 0;
}
