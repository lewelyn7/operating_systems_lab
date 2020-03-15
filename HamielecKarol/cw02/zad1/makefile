std: zad1.c
	gcc -g -Wall zad1.c -o zad1

test:
	make std
	./zad1 generate dane 10 3 lib
	cat dane
	./zad1 sort dane 10 3 lib
	cat dane

clean:
	rm zad1 dane*