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
