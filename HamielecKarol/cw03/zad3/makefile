std: macierz.c child.c program_pomocniczy.c
	gcc -g -Wall macierz.c -o macierz
	gcc -g -Wall child.c -o child
	gcc -g -Wall program_pomocniczy.c -o program_pomocniczy

cltest:
	echo -e "- - -\n- - -\n- - -" > c.txt

test:
	make clean
	make std
	./program_pomocniczy 5 10 8 create
	./macierz lista.txt 3 5 0 10 1000
	./program_pomocniczy 5 10 8 check
testbig:
	make clean
	make std
	./program_pomocniczy 3 4 3 create
	./macierz lista.txt 3 5 0 10 1000
	./program_pomocniczy 3 4 3 check

testbig2:
	make clean
	make std
	./program_pomocniczy 5 15 20 create
	./macierz lista.txt 3 10 1 10 1000
	./program_pomocniczy 5 15 20 check

test2:
	make clean
	make std
	./program_pomocniczy 3 4 3 create
	./macierz lista.txt 3 5 1 10 1000
	echo "now checking"
	./program_pomocniczy 3 4 3 check
clean:
	rm -fr tmp* macierz ?.txt* child program_pomocniczy