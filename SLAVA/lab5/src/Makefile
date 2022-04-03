all:
	gcc -lm -shared -fPIC -o lib1.so lib1.c
	gcc -lm -shared -fPIC -o lib2.so lib2.c

Prog1: main.o lib1.o
	gcc -lm main.o lib1.o -o Prog1

Prog2: din_main.c
	gcc din_main.c -o Prog2 -ldl

main.o: lib1.o
	gcc -c main.c

lib1.o: lib1.c
	gcc -c lib1.c

clean:
	rm -f *.o