# Bonus Marks

Exercises completed at the end of each chapter.

## Installation

* If no `main` function is given, the compiler will print an "undefined
  reference to `main`" error, like so:

        $ cc -std=c99 -Wall hello_world.c -o hello_world
        /usr/lib/gcc/i686-linux-gnu/4.8/../../../i386-linux-gnu/crt1.o: In function `_start':
        (.text+0x18): undefined reference to `main'
        collect2: error: ld returned 1 exit status

* `puts` can be found [here](http://en.cppreference.com/w/c/io/puts) in the
  online documentation.

## Basics

* Other conditional operators include `==`, `!=`, `>=`, and `<=`.

* Other mathematical operators include `*`, `/`, and `%`.

* The `+=` operator performs a compound assignment. `x += 2` would be equivalent
  to `x = x + 2`.

* The `do` loop executes some statement body while a condition is `true`.
  Because the condition is evaluated *after* the statement body, the body is
  guaranteed to always execute at least once.

* The `switch` statement is similar to an `if-else` conditional. An expression
  is compared to a number of possible constants, and if a match is found then
  the corresponding `case` statement body is executed.

* The `break` keyword marks the end of a `case` statement body, so that a body
  corresponding to a following `case` is not also executed.

* The `continue` keyword causes the containing loop to immediately jump to the
  next iteration, skipping any remaining statements for the current iteration.

* The `typedef` keyword allows you to give a type a new name.
