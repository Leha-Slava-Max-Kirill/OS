compile: Prog1 Prog2

Prog1: lib.h main.o lib1.o
	gcc main.o lib1.o -o Prog1
Prog2: lib1.so lib2.so din_main.c
	gcc din_main.c -ldl -o Prog2 -Wl,-rpath,.
main.o:
	gcc -c main.c
lib1.o: lib.h lib1.c
	gcc -c lib1.c -lm
lib1.so: lib1.c
	gcc -lm -shared -fPIC lib1.c  -o lib1.so -lm
lib2.so: lib2.c
	gcc -lm -shared -fPIC lib2.c  -o lib2.so -lm
clean:
	rm *.so *.o Prog1 Prog2