Some very useful links:
- about make: 
    http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
- about libraries:
    http://student.agh.edu.pl/~kzieba/
    http://student.agh.edu.pl/~bszczepa/sem4/sysopy/referat/
    http://home.agh.edu.pl/~gjn/dydaktyka/TechKomp/node14.html
- about function pointers:
    http://cpp0x.pl/kursy/Kurs-C++/Poziom-X/Wskaznik-na-funkcje/249

Notes:
- building static or dynamic library is quite simple, but with DLL you have to do some funny things.
	In file main.c there is some stuff with #ifdef directives. This is because for DLL you need
	pointers to each used function. But when you see an example, it becomes quite obvious, I hope.