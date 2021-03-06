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

## An Interactive Prompt

* `\n` is a newline character. The newline [implementation](https://en.wikipedia.org/wiki/Newline#Representations)
  varies between systems; for example, Windows might use `CR+LF` while OS X and
  Linux might use `LF` instead.
* `printf` has various [parameters](http://www.cplusplus.com/reference/cstdio/printf/)
  that can be used to format unsigned hexadecimal numbers in lowercase or
  uppercase, signed and unsigned decimal integers, characters, strings, and
  more. Width sub-specifiers, for example, can also be used to pad some output
  with spaces.
* Writing a syntactically invalid call to `printf` returns the following error
  from the compiler:

        $ cc -std=c99 -Wall prompt.c -ledit -o prompt.o
        prompt.c: In function ‘main’:
        prompt.c:38:5: warning: format ‘%u’ expects argument of type ‘unsigned int’, but argument 2 has type ‘char *’ [-Wformat=]
             printf("Invalid printf attempt: %u", "H");
             ^
        prompt.c:39:5: warning: format ‘%u’ expects argument of type ‘unsigned int’, but argument 2 has type ‘char *’ [-Wformat=]
             printf("Invalid printf attempt: %u", "Hello");
             ^
    The [`printf` reference](http://www.cplusplus.com/reference/cstdio/printf/)
    indicates that in the case of a writing error, the error indicator
    (`ferror`) is set and a negative number is returned.
* The `#ifndef` preprocessor command checks if some token has been defined; if
  not, the code between it and `#endif`, or `#else` if present, is included.
* The `#define` preprocessor command is used to create an [object-like macro](https://gcc.gnu.org/onlinedocs/cpp/Object-like-Macros.html).
  A macro is a fragment of code which has been given a name; whenever the name
  is used, it is replaced by the contents of the macro.
* Linux and Mac OS X might define the `__linux__` and `__APPLE__&&__MACH__`
  macros, respectively.

## Languages

* Other examples of strings the `Doge` language includes are "`wow lisp`",
  "`many c wow language`," and "`many build`."
* Back slashes escape the quotation marks, which delineate literal words in the
  re-write rules.
* Back slashes at the end of the line serve to wrap the string to the next
  line.
* First, create a rule defining `digit`, allowing exactly one number (character)
  between zero and nine, inclusive. Then create another rule for `decimal`,
  allowing one or more `digit`, followed by exactly one character `.` period,
  followed by one or more `digit` again.
* A grammar for web URLs might include rules for `protocol` (`https?`), `domain`
  (an alphanumeric string), `tld` (`com`, `net`, or `edu`), and `url`
  (`protocol`, "`://`", `domain`, '`.`', `tld`).
* A grammar for simple English sentences might include rules for an article,
  noun, noun phrase, verb, and verb phrase.
* A grammar for a decimal number:

        digit ::= [0-9]
        decimal-number ::= <digit>+ "." <digit>+
    A grammar for a web address:

        protocol ::= "http" | "https"
        domain ::= [A-Za-z0-9][-A-Za-z0-9]*
        tld ::= "com" | "net" | "edu"
        url ::= <protocol> "://" (<domain> ".")* <domain> "." <tld>
    A grammar for simple English sentences:

        article ::= "a" | "an" | "the"
        noun ::= "cat" | "mat" | "boy"
        noun-phrase ::= <article> <noun>
        verb ::= "sat" | "meowed" | "slept"
        preposition ::= "on" | "beneath" | "near"
        verb-phrase ::= <verb> <preposition> <noun-phrase>
        sentence ::= <noun-phrase> <verb-phrase>

## Parsing

* A regular expression to match strings of all `a` or `b` is `[ab]+`.
* A regular expression to match strings of consecutive `a` and `b` is
  `a(ba)*b?`.
* A regular expression to match these strings is `^(pit|pot|respite)$`.
  While attempting to change the grammar to parse decimal numbers, I encountered
  the following error at compile time:

        $ cc -std=c99 -Wall parsing.c mpc.c -ledit -lm -o parsing.o
        parsing.c: In function ‘main’:
        parsing.c:43:13: warning: unknown escape sequence: '\.' [enabled by default]
                  "                                                     \
                  ^
    The corresponding code:

        number   : /-?[0-9]+(.[0-9]+)?/ ;                                                                                                                                      
    Does `mpc` not support escaping characters (as a `.` usually matches any
    character, when we want to match only a literal dot.

