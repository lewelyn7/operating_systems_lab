std: macierz.c child.c
	gcc -g -Wall macierz.c -o macierz
	gcc -g -Wall child.c -o child

cltest:
	echo -e "- - -\n- - -\n- - -" > c.txt

test:
	make std
	make cltest
	./macierz lista 3 5 5
clean:
	rm tmp* macierz 