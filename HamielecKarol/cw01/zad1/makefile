static: zad1.c zad1.h
	make clean
	gcc -Wall -c -g -I . zad1.c
	ar crs libzad1.a zad1.o
shared: zad1.c zad1.h
	make clean
	gcc -Wall -fPIC -c -I . zad1.c
	gcc -Wall -shared -fPIC -o libzad1.so zad1.o

clean:
	rm -f *.o *.a *.so zad1
