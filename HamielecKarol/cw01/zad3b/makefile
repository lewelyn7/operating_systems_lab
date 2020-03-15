CC = gcc -Wall -g
static: main.c zad1.c zad1.h
	make clean
	$(CC) -c -I . zad1.c
	ar crs libzad1.a zad1.o
	$(CC) main.c  -o main -L. -lzad1 -O$(Onum)

shared: main.c zad1.c zad1.h
	make clean
	$(CC) -fPIC -c -I . zad1.c
	$(CC) -shared -fPIC -o libzad1.so zad1.o
	gcc main.c -Wall -o main -L. -lzad1 -Wl,-rpath=`pwd` -O$(Onum)
dll: main.c zad1.c zad1.h
	make clean
	$(CC)  -fPIC -I . -c zad1.c -o zad1.o 
	gcc -Wl,-rpath=. -fPIC -shared -o libzad1.so zad1.o 	
	$(CC) -fPIC -I . -ldl -L. -o main main.c -D DYNAMIC -O$(Onum)
clean:
	rm -rf *.o *.a *.so main tmp/* zad1
test: main

	./main create_table 50 compare_pairs a.txt:b.txt dlugi1.txt:dlugi2.txt biblia.txt:bible.txt remove_block 1 remove_block 2 remove_block 3 compare pairs dlugi2.txt:lalka.txt dlugi2.txt:3muszkiet.txt lalka.txt:a.txt lalka.txt:b.txt lalka.txt:biblia.txt lalka.txt:bible.txt lalka.txt:dlugi1.txt lalka.txt:dlugi2.txt lalka.txt:3muszkiet.txt 3muszkiet.txt:a.txt 3muszkiet.txt:b.txt 3muszkiet.txt:biblia.txt 3muszkiet.txt:bible.txt 3muszkiet.txt:dlugi1.txt 3muszkiet.txt:dlugi2.txt 3muszkiet.txt:lalka.txt compare_pairs a.txt:b.txt a.txt:biblia.txt a.txt:bible.txt a.txt:dlugi1.txt a.txt:dlugi2.txt a.txt:lalka.txt a.txt:3muszkiet.txt b.txt:a.txt b.txt:biblia.txt b.txt:bible.txt b.txt:dlugi1.txt b.txt:dlugi2.txt b.txt:lalka.txt b.txt:3muszkiet.txt biblia.txt:a.txt biblia.txt:b.txt biblia.txt:bible.txt biblia.txt:dlugi1.txt biblia.txt:dlugi2.txt biblia.txt:lalka.txt biblia.txt:3muszkiet.txt bible.txt:a.txt bible.txt:b.txt bible.txt:biblia.txt bible.txt:dlugi1.txt bible.txt:dlugi2.txt bible.txt:lalka.txt bible.txt:3muszkiet.txt dlugi1.txt:a.txt dlugi1.txt:b.txt dlugi1.txt:biblia.txt dlugi1.txt:bible.txt dlugi1.txt:dlugi2.txt dlugi1.txt:lalka.txt dlugi1.txt:3muszkiet.txt dlugi2.txt:a.txt dlugi2.txt:b.txt dlugi2.txt:biblia.txt dlugi2.txt:bible.txt dlugi2.txt:dlugi1.txt remove_block 2 remove_block 10 remove_block 11 remove 21 remove_block 33 compare_pairs dlugi1.txt:dlugi2.txt remove_block 2 compare_pairs a.txt:b.txt remove_block 2 compare_pairs biblia.txt:bible.txt remove_block 2 lalka.txt:bible.txt

tests: main
	make static
	make test
	mv raport2.txt result3b.txt
	make shared
	make test
	echo "NOW SHARED LIBRARY" >> result3b.txt
	make test
	cat raport2.txt >> result3b.txt
	sed -i '1i#define DLL' main.c
	make dll
	tail -n +2 main.c > tmp.txt
	cat tmp.txt > main.c
	echo "NOW DLL LIBRARY" >> result3b.txt
	make test
	cat raport2.txt >> result3b.txt
	rm raport2.txt tmp.txt


	