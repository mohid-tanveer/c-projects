# introduction

In this project, I used dynamic memory management in C to implement a
hash table.

The objectives of this project were:

* to gain experience writing programs that explicitly manage their own memory
  rather than relying on a garbage collection;
* avoiding any memory leaks;
* to gain experience writing real, useful general-purpose library code that will
  be used by client programs;
* to gain experience programming to an interface;
* to implement a linked data structure in C;
* and to learn about hash tables and one implementation of them.

# overview

```
project2
|-- README.md  ................ This document
`-- src ....................... Main source code directory
    |-- books ................. Files used for word count test
    |   |-- alice...txt 
    |   |-- ...
    |   `-- walde...txt
    |-- fnv64.h ............... The hash function used
    |-- interactive_driver.c .. Test program to interact manually with a map
    |-- Makefile .............. Build rules
    |-- map.c ................. Main source code file for the map
    |-- map.h ................. Header file for map; function declarations
    |-- rand1.h ............... Random number generator used in tests
    |-- reference ............. Test programs compiled with reference solution
    |   |-- idriver 
    |   |-- stress_test
    |   `-- word_count
    |-- stress_test.c ......... Test that stresses the map implementation
    |-- tests ................. Unit tests
    |   |-- airport_codes.c ... Airport codes used in tests
    |   |-- airport_codes.h ... Airport codes used in tests
    |   |-- fnv64_test.c ...... Unit tests for hash function
    |   |-- map_test.c ........ Main unit test file
    |   |-- test_utils.c ...... Unit test library for this class
    |   `-- test_utils.h ...... Unit test library for this class
    `-- word_count.c .......... Word frequency count test program
```