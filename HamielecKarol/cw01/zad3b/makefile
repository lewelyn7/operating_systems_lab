static:
	make clean
	gcc -Wall -c -g -I . zad1.c
	ar crs libzad1.a zad1.o
	gcc -Wall main.c -g -o main -L. -lzad1

dynamic:
	make clean
	gcc -Wall -fPIC -c -I . zad1.c
	gcc -Wall -shared -fPIC -o libzad1.so zad1.o
	gcc main.c -Wall -o main -L. -lzad1 -Wl,-rpath=`pwd`
dll:
	make clean
	gcc -Wall -g -fPIC -I . -c zad1.c -o zad1.o 
	gcc -Wl,-rpath=. -fPIC -shared -o libzad1.so zad1.o 	
	gcc -Wall -g -fPIC -I . -ldl -L. -o main main.c -D DYNAMIC
clean:
	rm -f *.o *.a *.so main
test:
	./main create_table 50 compare_pairs a.txt:b.txt dlugi1.txt:dlugi2.txt biblia.txt:bible.txt remove_block 1 remove_block 2 remove_block 3 compare pairs dlugi2.txt:lalka.txt dlugi2.txt:3muszkiet.txt lalka.txt:a.txt lalka.txt:b.txt lalka.txt:biblia.txt lalka.txt:bible.txt lalka.txt:dlugi1.txt lalka.txt:dlugi2.txt lalka.txt:3muszkiet.txt 3muszkiet.txt:a.txt 3muszkiet.txt:b.txt 3muszkiet.txt:biblia.txt 3muszkiet.txt:bible.txt 3muszkiet.txt:dlugi1.txt 3muszkiet.txt:dlugi2.txt 3muszkiet.txt:lalka.txt compare_pairs a.txt:b.txt a.txt:biblia.txt a.txt:bible.txt a.txt:dlugi1.txt a.txt:dlugi2.txt a.txt:lalka.txt a.txt:3muszkiet.txt b.txt:a.txt b.txt:biblia.txt b.txt:bible.txt b.txt:dlugi1.txt b.txt:dlugi2.txt b.txt:lalka.txt b.txt:3muszkiet.txt biblia.txt:a.txt biblia.txt:b.txt biblia.txt:bible.txt biblia.txt:dlugi1.txt biblia.txt:dlugi2.txt biblia.txt:lalka.txt biblia.txt:3muszkiet.txt bible.txt:a.txt bible.txt:b.txt bible.txt:biblia.txt bible.txt:dlugi1.txt bible.txt:dlugi2.txt bible.txt:lalka.txt bible.txt:3muszkiet.txt dlugi1.txt:a.txt dlugi1.txt:b.txt dlugi1.txt:biblia.txt dlugi1.txt:bible.txt dlugi1.txt:dlugi2.txt dlugi1.txt:lalka.txt dlugi1.txt:3muszkiet.txt dlugi2.txt:a.txt dlugi2.txt:b.txt dlugi2.txt:biblia.txt dlugi2.txt:bible.txt dlugi2.txt:dlugi1.txt remove_block 2 remove_block 10 remove_block 11 remove 21 remove_block 33 compare_pairs dlugi1.txt:dlugi2.txt remove_block 2 compare_pairs a.txt:b.txt remove_block 2 compare_pairs biblia.txt:bible.txt remove_block 2 lalka.txt:bible.txt