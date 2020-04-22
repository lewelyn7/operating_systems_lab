std: worker1.c worker2.c worker3.c utils.h
	gcc -g -Wall worker1.c -o worker1.out -lrt
	gcc -g -Wall worker2.c -o worker2.out -lrt
	gcc -g -Wall worker3.c -o worker3.out -lrt
	gcc -g -Wall mother.c -o mother.out -lrt

clean:
	rm *.out