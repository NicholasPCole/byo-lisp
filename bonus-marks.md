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
