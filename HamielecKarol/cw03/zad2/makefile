std: macierz.c child.c program_pomocniczy.c
	gcc -g -Wall macierz.c -o macierz
	gcc -g -Wall child.c -o child
	gcc -g -Wall program_pomocniczy.c -o program_pomocniczy

cltest:
	echo -e "- - -\n- - -\n- - -" > c.txt

test:
	make clean
	make std
	./program_pomocniczy 3 4 3 create
	./macierz lista.txt 3 5 0
	./program_pomocniczy 3 4 3 check
test2:
	make clean
	make std
	./program_pomocniczy 3 4 3 create
	./macierz lista.txt 3 5 1
	echo "now checking"
	./program_pomocniczy 3 4 3 check
clean:
	rm -fr tmp* macierz ?.txt*