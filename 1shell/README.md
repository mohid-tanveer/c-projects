# Shell

In this project I created a shell in C.

This shell interprets user commands and performs delegation to the operating system
on behalf of the user.

The shell provides the basic functionality necessary for a user to interact
with any system's(Unix or Windows) terminal.

It essentially runs a loop:

1. Prompts the user for an input command.
2. Parse the input line in order to determine whether to run another program or
   perform a built-in action.
3. Either perform the built-in, or
4. Fork itself and, in the child process, execute a program.
5. Wait for the program or action to exit.
6. Repeat.

Skills associated with this project are:

* Familiarity with and using string processing functions in the C library.
* Understanding and manipulating pointers to data.
* C syntax and semantics.
* Debugging a nontrivial C program.

## example interaction

[Here's](doc/sample_interaction.md) what a sample interaction within the shell should
look like.

## overview

The structure of the project is as follows:

```text
1shell
|-- doc                     -- references in this file
|-- README.md               -- this file
`-- src          
    |-- argument_test.c     -- test program for running relative path programs
    |-- cwd.c               -- library function for command loop 
    |-- cwd.h               -- library function for command loop
    |-- environment_test.c  -- test program for running relative path programs
    |-- project1.c              -- main source code file
    `-- Makefile            -- make rule definitions
```

The source code for the project is in the `src` directory.

## features

* Run programs, including programs that take command line arguments.
* Allow a user to use `cd` to navigate the filesystem structure.
* Keep a history of commands and allow the user to view their command history
  and optionally re-execute previously-executed commands.
