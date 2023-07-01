# Hash Map

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

## example interactions

### idriver

```bash
$ make idriver
gcc -g -Wall -Werror   -c -o interactive_driver.o interactive_driver.c
gcc -g -Wall -Werror   -c -o map.o map.c
gcc -g -o idriver interactive_driver.o map.o

$ ./idriver
Interactive map tester: ? for help.
Commands:
        n <size>            create or reallocate map
        p <key> <value>     map[key] = value
        g <key>             get map[key]
        r <key>             remove map[key]
        f                   free map
        v                   dump map contents (tests apply_fn)
        s                   print map stats
        d                   call map debug function
        ?                   print this message
        e                   exit tester
> n 4
Allocating map with size 4...done.
> p SFO San-Francisco
Putting map[SFO] -> San-Francisco...done (is_new = 1)
> p PHL Philadelphia
Putting map[PHL] -> Philadelphia...done (is_new = 1)
> p MEM Memphis
Putting map[MEM] -> Memphis...done (is_new = 1)
> p SJC San-Jose
Putting map[SJC] -> San-Jose...done (is_new = 1)
> p CMH Cooooooooolumbus
Putting map[CMH] -> Cooooooooolumbus...done (is_new = 1)
> d
Calling map_debug...
0 d847a1186b314444 MEM(Memphis) -> X
1 d832be186b1f1909 SFO(San-Francisco) -> d832ca186b1f2de9 SJC(San-Jose) -> X
2  -> X
3 d83cea186b27b8c3 PHL(Philadelphia) -> d869a5186b4e3173 CMH(Cooooooooolumbus) -> X
done
> g ATL
Getting ATL...done; value = NULL (exists = 0)
> g MEM
Getting MEM...done; value = Memphis (exists = 1)
> g CMH
Getting CMH...done; value = Cooooooooolumbus (exists = 1)
> p CMH Columbus
Putting map[CMH] -> Columbus...done (is_new = 0)
> r PHL
Removing PHL...done (exists = 1)
> d
Calling map_debug...
0 d847a1186b314444 MEM(Memphis) -> X
1 d832be186b1f1909 SFO(San-Francisco) -> d832ca186b1f2de9 SJC(San-Jose) -> X
2  -> X
3 d869a5186b4e3173 CMH(Columbus) -> X
done
> n 1
Reallocating map to size 1...done.
> d
Calling map_debug...
0 d847a1186b314444 MEM(Memphis) -> d832be186b1f1909 SFO(San-Francisco) -> d832ca186b1f2de9 SJC(San-Jose) -> d869a5186b4e3173 CMH(Columbus) -> X
done
> n 6
Reallocating map to size 6...done.
> d
Calling map_debug...
0  -> X
1 d832be186b1f1909 SFO(San-Francisco) -> d869a5186b4e3173 CMH(Columbus) -> X
2 d847a1186b314444 MEM(Memphis) -> X
3  -> X
4  -> X
5 d832ca186b1f2de9 SJC(San-Jose) -> X
done
> q
```

### word_count and stress_test

```bash
$ ./word_count
Usage: word_count -f <file> [-s <size>] [-d] [-r]
Options:
        -f <file>  Count words in <file>
        -s <size>  Size of map
        -d         Print map debug stats
        -r         Periodically resize between 1 and 8k entries (stress test)
$ ./word_count -f books/alice-in-wonderland.txt | sort -nr | head
1632       the
845        and
721        to
627        a
537        she
526        it
508        of
462        said
401        i
386        alice
$ make run_word_count
----------------------------------
Map Stats:
----------------------------------
num_entries:    2750
max_depth:      8
size:           1612
resize count:   2564
==53192==
==53192== HEAP SUMMARY:
==53192==     in use at exit: 0 bytes in 0 blocks
==53192==   total heap usage: 9,029,078 allocs, 9,029,078 frees, 297,501,813 bytes allocated
==53192==
==53192== All heap blocks were freed -- no leaks are possible
==53192==
==53192== For counts of detected and suppressed errors, rerun with: -v
==53192== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
$ make run_stress_test
==54935== HEAP SUMMARY:
==54935==     in use at exit: 0 bytes in 0 blocks
==54935==   total heap usage: 40,887,203 allocs, 40,887,203 frees, 942,280,717
bytes allocated
==54935==
==54935== All heap blocks were freed -- no leaks are possible
==54935==
==54935== For counts of detected and suppressed errors, rerun with: -v
==54935== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

## overview

```text
2hashmap
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
