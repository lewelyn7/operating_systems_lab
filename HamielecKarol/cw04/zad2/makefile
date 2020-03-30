std: zad2.c exec_child.c
	gcc -g -Wall zad2.c -o zad2
	gcc -g -Wall exec_child.c -o exec_child

test:
	make std
	echo "--------------------" >> raport2.txt
	./zad2 handler $(signal) fork >> raport2.txt

	./zad2 ignore $(signal) exec >> raport2.txt
	./zad2 ignore $(signal) fork >> raport2.txt

	./zad2 mask $(signal) exec >> raport2.txt
	./zad2 mask $(signal) fork >> raport2.txt

	# ./zad2 pending $(signal) exec >> raport2.txt
	# ./zad2 pending $(signal) fork >> raport2.txt
	echo "--------------------" >> raport2.txt

tests:
	echo " " > raport2.txt
	make test signal=SIGINT
	make test signal=SIGQUIT
	# make test signal=SIGKILL
	make test signal=SIGTERM
	make test signal=SIGCHLD
	make test signal=SIGUSR1
	
clean:
	rm zad2 exec_child