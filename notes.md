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
