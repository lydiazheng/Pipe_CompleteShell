all: testfile

testfile: my_shell.o
	gcc -o testfile my_shell.o

testfile.o: my_shell.c 
	gcc -c my_shell.c

clean:
	rm -f testfile *.o my_shell.o