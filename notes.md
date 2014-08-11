# Notes

Various snippets, mostly references and notes to myself, as I progress through
the chapters of the book.

## Introduction

* Build Your Own Lisp is partially inspired by tutorials like [Write Yourself a Scheme in 48 Hours](http://en.wikibooks.org/wiki/Write_Yourself_a_Scheme_in_48_Hours).
  Having worked with Scheme in a previous course with [Stephen L. Bloom](http://www.cs.stevens.edu/~bloom/),
  this is a tutorial that I might be interested in after byo-lisp.

## Installation

* On Ubuntu 14.04, the following is output:

        $ cc --version
        cc (Ubuntu/Linaro 4.8.1-10ubuntu9) 4.8.1
        Copyright (C) 2013 Free Software Foundation, Inc.
        This is free software; see the source for copying conditions.  There is NO
        warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    On OS X 10.9.4, the following is output:

        $ cc --version
        Apple LLVM version 5.1 (clang-503.0.40) (based on LLVM 3.4svn)
        Target: x86_64-apple-darwin13.3.0
        Thread model: posix
* The Compilation section prompts the user to run the following command:

        cc -std=c99 -Wall hello_world.c -o hello_world
    Not explained is the `-Wall` flag. According to [FreeBSD documentation](http://www.freebsd.org/doc/en/books/developers-handbook/tools-compiling.html),
    this tells `cc` to enable warnings which the package's authors believe worthwhile.

## Basics

* C programs consist of *function definitions* and *structure definitions*.
  Source files contain *functions* and *types*. Structures are used to declare
  new types, like so:

        typedef struct {
            float x;
            float y;
        } point;
* In C, strings are represented by the pointer type `char*`. Built-in types
  include `void`, `char`, `int`, `long`, `float`, `double`, and more.

## An Interactive Prompt

* There is a typo in the first paragraph: "we can built" should be "we can
  build". If I find many more typos in the rest of the book, I will submit a
  pull request fixing them.
* While writing a `while(true) {}` loop, the compiler returned an error:

        $ cc -std=c99 -Wall prompt.c -o prompt.o
        prompt.c: In function ‘main’:
        prompt.c:10:12: error: ‘true’ undeclared (first use in this function)
             while (true) {
                    ^
        prompt.c:10:12: note: each undeclared identifier is reported only once for each function it appears in
    This occurs because C does not have any built-in Boolean types.
* Lines beginning with a `#` character are preprocessor commands.
* There is a typo in the Bonus Marks section: "with `printf`." should be "with
  `printf`?"

## Languages

* *Re-write rules* describe how one thing can be re-written as something else.
  Although an infinite number of things (or sentences) can be expressed in a
  language, it is possible to process and understand them using a finite number
  of re-write rules, collectively known as a *grammar*.
* [`mpc`](https://github.com/orangeduck/mpc) is a Parser Combinator library; it
  enables us to write normal code that just looks like a grammar, or we can use
  special syntax to write a grammar directly.
* There is a typo in the first paragraph of the Coding Grammars section: "Shiba
  Inu" should be followed by a comma instead of a period, and "More" switched to
  lower-case.
* `mpc` functions for building parsers include `mpc_or`, `mpc_and`, and
  `mpc_many`. `mpcf_strfold` and `free` are two arguments that we will ignore
  for now.

## Parsing

* When including files in C source code, surrounding the file name with angled
  brackets will search system locations for headers first, while surrounding the
  file name with quotation marks searches the current directory first.
* What is the purpose of the first argument, `"<stdin>"`, in the `mpc_parse`
  function?
* There is a typo in the "Parsing User Input" section. "On success a internal
  structure" should be "On success an internal structure."


## Evaluation

* Abstract syntax trees are represented in `mpc` by the `mpc_ast_t` structure,
  with fields for the `tag`, `contents`, `state`, `children_num`, and `children`,
* Note that `children` is a double pointer type. Because the type `mpc_ast_t*`
  is a pointer to a structure, you use `->` to access its fields.

## Error Handlings

* Two useful programs for debugging are `gdb` and `valgrind`.
* The question mark in `<condition> ? <then> : <else>` is known as the *ternary
  operator*.
* In what instances will `LERR_BAD_OP` or `LERR_BAD_NUM` error types be created,
  since parsing should find those values not matching the language and thus not
  perform the computation anyway?

## S-Expressions

* In this chapter, we will extend our `lval` structure to represent an
  S-Expression, or *Symbolic Expression*. S-Expressions are evaluated in a
  manner similar to Lisps: look at the first item in the list and take this to
  be the operator; then look at all other items in the list and them to be the
  operands.
* Arguments are passed *by value* for `int`, `long`, `char`, and user-defined
  `struct` types such as `lval`.
* There is a typo in the second paragraph of the "Pointers" section: "user
  defined" should be "user-defined."
* Pointer types are declared by suffixing existing ones with the `*` character.
  To get the address of some data, prefix the variable name with the `&`
  character. To get the data at an address, prefix the variable name with the
  `*` operator.
* There is a typo in the last paragraph of the "Pointers" section: "size" should
  be "side."
* The stack is the temporary memory area used while your program - and different
  functions within - are running. The heap is a section of memory used for
  long-term storage, and modified using the `malloc` and `free` functions.
* There is a typo in the first paragraph of the "Parsing Expressions" section:
  "parenthesis" should be "parentheses."

## Q-Expressions

* Adding new features to a language typically follows these four steps:
    * Syntax: Add a new rule to the language grammar for this feature.
    * Representation: Add new data type variations to represent this feature.
    * Parsing: Add new functions for reading this feature from the abstract
      syntax tree.
    * Semantics: Add new functions for evaluating and manipulating this feature.
* Q-expressions, or *quoted expressions*, are not evaluated by standard Lisp
  mechanics. Their syntax is similar to that of S-expressions, but surrounded
  by curly brackets `{}` instead.

## Variables

* The *environment* is a structure which stores the name and value of everything
  named in our program.
* Once variables are introduced, we will not need to explicitly add symbols to
  the grammar. Instead, they will be a name for us to lookup in the environment.
* Therefore, we will need a new value to represent functions. *Function pointers*
  have a type matching the type of the function pointed to, not the type of the
  data pointed to.
* Because both the `lval` and `lenv` structs depend on each other, there is a
  *cyclic dependency* that must be addressed.
