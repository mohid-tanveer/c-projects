# example interaction

```bash
mohidtanveer@mohids-MacBook-Pro src % make
gcc -g -Wall -Werror   -c -o project1.o project1.c
gcc -g -Wall -Werror   -c -o cwd.o cwd.c
gcc -g -o my_shell project1.o cwd.o
mohidtanveer@mohids-MacBook-Pro src % ./my_shell
Welcome to MyShell!
PATH:   /opt/homebrew/opt/ncurses/bin
        /opt/homebrew/bin
        /opt/homebrew/sbin
        /usr/local/bin
        /usr/bin
        /bin
        /usr/sbin
        /sbin
        /usr/local/share/dotnet
        ~/.dotnet/tools
$ pwd
/Users/mohidtanveer/Documents/Comp Sci Portfolio/c-projects/c-projects/1shell/src
$ cd ..
$ pwd
/Users/mohidtanveer/Documents/Comp Sci Portfolio/c-projects/c-projects/1shell
$ cd src
$ cat Makefile
CFLAGS=-g -Wall -Werror
CC=gcc

my_shell: project1.o cwd.o
        $(CC) -g -o $@ $^

environment_test: environment_test.o
        $(CC) -g -o $@ $^

argument_test: argument_test.o
        $(CC) -g -o $@ $^

clean:
        rm -f *.o argument_test environment_test my_shell
$ history
history of commands: 
0: pwd
1: cd ..
2: pwd
3: cd src
4: cat Makefile
$ !4
cat Makefile
CFLAGS=-g -Wall -Werror
CC=gcc

my_shell: project1.o cwd.o
        $(CC) -g -o $@ $^

environment_test: environment_test.o
        $(CC) -g -o $@ $^

argument_test: argument_test.o
        $(CC) -g -o $@ $^

clean:
        rm -f *.o argument_test environment_test my_shell
$ !0
pwd
/Users/mohidtanveer/Documents/Comp Sci Portfolio/c-projects/c-projects/1shell/src
$ make environment_test
gcc -g -Wall -Werror   -c -o environment_test.o environment_test.c
gcc -g -o environment_test environment_test.o
$ ./environment_test
OK 12 /opt/homebrew/opt/ncurses/bin:/opt/homebrew/bin:/opt/homebrew/sbin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/share/dotnet:~/.dotnet/tools:/opt/homebrew/opt/ncurses/bin:/opt/homebrew/bin:/opt/homebrew/sbin
$ ls -al
total 256
drwxr-xr-x  14 mohidtanveer  staff    448 Jul  7 20:03 .
drwxr-xr-x   7 mohidtanveer  staff    224 Jun 20 12:13 ..
-rw-r--r--   1 mohidtanveer  staff   6148 Jun 20 12:13 .DS_Store
-rwxr-xr-x   1 mohidtanveer  staff    245 Jun 20 12:13 Makefile
-rwxr-xr-x   1 mohidtanveer  staff    200 Jun 20 12:13 argument_test.c
-rwxr-xr-x   1 mohidtanveer  staff    143 Jun 20 12:13 cwd.c
-rwxr-xr-x   1 mohidtanveer  staff     71 Jun 20 12:13 cwd.h
-rw-r--r--   1 mohidtanveer  staff   2200 Jul  7 20:03 cwd.o
-rwxr-xr-x   1 mohidtanveer  staff  33901 Jul  7 20:03 environment_test
-rwxr-xr-x   1 mohidtanveer  staff    485 Jun 20 12:13 environment_test.c
-rw-r--r--   1 mohidtanveer  staff   2856 Jul  7 20:03 environment_test.o
-rwxr-xr-x   1 mohidtanveer  staff  35877 Jul  7 20:03 my_shell
-rw-r--r--   1 mohidtanveer  staff   6955 Jul  7 20:02 project1.c
-rw-r--r--   1 mohidtanveer  staff  11544 Jul  7 20:03 project1.o
$ make clean
rm -f *.o argument_test environment_test my_shell
$ exit
mohidtanveer@mohids-MacBook-Pro src % 
```
