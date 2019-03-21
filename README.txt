Weiling Zheng
301294091  weilingz@sfu.ca

Introduction:
The program implements my own Linux shell with few internal commands and a set of fully functioning external commands. C language is the only programming languages used in the project due to operating system constraints. The latest feature, pipes, is integrated into the shell program to empower the efficiency of Linux shell. The following sections in readme file will provide compile instructions, supported commands, and sample outputs.


Instruction to Compile:
1. type 'make'
2. type './testfile'
3. then it will show my_shell: CMPT300@my_shell:~$
4. input regular Linux commands to run this program
5. type 'exit' or use ctrl+D to quit my_shell


Features that have been implemented:
1. internal commands: exit, cd, pwd, history
2. external commands
3. pipes(e.g., cat textfile | gzip -c | gunzip -c | tail -n 10)


The output sample:

weilingz@asb9838nu-b12:~/Desktop/pipe$ make
make: Nothing to be done for `all'.
weilingz@asb9838nu-b12:~/Desktop/pipe$ ./testfile
CMPT300@my_shell:~$ ls
makefile  my_shell.c  my_shell.o  README.txt  sss  testfile  textfile
CMPT300@my_shell:~$ cd sss
CMPT300@my_shell:~$ cd ..
CMPT300@my_shell:~$ pwd
Directory:  /home/weilingz/Desktop/pipe
CMPT300@my_shell:~$ ls
makefile  my_shell.c  my_shell.o  README.txt  sss  testfile  textfile
CMPT300@my_shell:~$ cat textfile | gzip -c | gunzip -c | tail -n 10
5
6
7
8
9
10
11
12
13
14
CMPT300@my_shell:~$ ps -ax | grep bash
12390 pts/1    Ss     0:00 bash
12426 pts/1    S+     0:00 grep bash
CMPT300@my_shell:~$ history 

   1 ls
   2 cd sss
   3 cd ..
   4 pwd
   5 ls
   6 cat textfile | gzip -c | gunzip -c | tail -n 10
   7 ps -ax | grep bash
   8 history

CMPT300@my_shell:~$ exit
Wow! my_shell exit!
