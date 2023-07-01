```
$ make
gcc -g -Wall -Werror   -c -o project1.o project1.c
gcc -g -Wall -Werror   -c -o cwd.o cwd.c
gcc -g -o my_shell project1.o cwd.o
$ ./my_shell 
Welcome to MyShell!
Path: 	/home
	/opt/hpc/bin
	/usr/local/Modules/bin
	/usr/local/sbin
	/usr/local/bin
	/usr/sbin
	/usr/bin
	/sbin
	/bin
	/usr/games
	/usr/local/games
	/snap/bin
	/usr/local/go/bin
$ pwd
/home/project1/src
$ cd ..
$ pwd
/home/project1
$ cd src
$ cat Makefile
CFLAGS=-g -Wall -Werror
CC=gcc

my_shell: project1.o cwd.o
	$(CC) -g -o $@ $^

environment_test: environment_test.o
	$(CC) -g -o $@ $^

clean:
	rm -f *.o environment_test my_shell
$ history
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

clean:
	rm -f *.o environment_test my_shell
$ !0
pwd
/home/project1/src
$ make environment_test
gcc -g -Wall -Werror   -c -o environment_test.o environment_test.c
gcc -g -o environment_test environment_test.o
$ ./environment_test
OK 12 /home/bin:/opt/hpc/bin:/usr/local/Modules/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/usr/local/go/bin
$ ls -al
total 92
drwxrwx--- 2 mohidtanveer mohidtanveer  4096 Sep  6 17:59 .
drwxrwx--- 4 mohidtanveer mohidtanveer  4096 Sep  6 17:57 ..
-rw-rw---- 1 mohidtanveer mohidtanveer   143 Sep  5 22:55 cwd.c
-rw-rw---- 1 mohidtanveer mohidtanveer    71 Sep  5 22:56 cwd.h
-rw-rw---- 1 mohidtanveer mohidtanveer  3640 Sep  6 17:58 cwd.o
-rwxrwx--- 1 mohidtanveer mohidtanveer 11240 Sep  6 17:59 environment_test
-rw-rw---- 1 mohidtanveer mohidtanveer   485 Sep  5 22:38 environment_test.c
-rw-rw---- 1 mohidtanveer mohidtanveer  6896 Sep  6 17:59 environment_test.o
-rw-rw---- 1 mohidtanveer mohidtanveer  4398 Sep  5 23:30 project1.c
-rw-rw---- 1 mohidtanveer mohidtanveer 14848 Sep  6 17:58 project1.o
-rw-rw---- 1 mohidtanveer mohidtanveer   176 Sep  6 17:50 Makefile
-rwxrwx--- 1 mohidtanveer mohidtanveer 19040 Sep  6 17:58 my_shell
$ exit
$
```

