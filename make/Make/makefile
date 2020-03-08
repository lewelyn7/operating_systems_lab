#hash means comment
#label ends with ':'
#you have to indent commands for each label
#to make your project, type 'make <label>' in shell

simple:	#simply compiles all files together
	gcc -Wall sayHello.c saySomething.c main.c -o main

o:	#first compiles .c files to .o files, then compiles it together with main file
	gcc -Wall -c sayHello.c
	gcc -Wall -c saySomething.c
	gcc -Wall sayHello.o saySomething.o main.c -o main

smart:	#cleans up before and after compilation
	make clean
	make o
	rm -f *.o

clean:	#removes everything apart from source files; it's nice to have such cleaning command
	rm -f *.o main

