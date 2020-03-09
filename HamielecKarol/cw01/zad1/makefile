static:
	make clean
	gcc -Wall -c -g -I . zad1.c
	ar crs libzad1.a zad1.o
	gcc -Wall main.c -g -o main -L. -lzad1

dynamic:
	make clean
	gcc -Wall -c -I . zad1.c
	gcc -Wall -shared -fPIC -o libzad1.so zad1.o
	gcc main.c -I . -Wall -o main -L. -lsay -Wl,-rpath=`pwd`

clean:
	rm -f *.o *.a *.so main
