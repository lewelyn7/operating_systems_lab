std: zad1.c
	gcc -g -Wall zad1.c -o zad1

test:
	make std
	# 1
	./zad1 generate tmp1 10000 1 lib
	./zad1 copy tmp1 tmp1sys 10000 1 sys
	./zad1 copy tmp1 tmp1lib 10000 1 lib
	./zad1 sort tmp1sys 10000 1 sys
	./zad1 sort tmp1lib 10000 1 lib
	# 4
	./zad1 generate tmp4 10000 4 lib
	./zad1 copy tmp4 tmp4sys 10000 4 sys
	./zad1 copy tmp4 tmp4lib 10000 4 lib
	./zad1 sort tmp4sys 10000 4 sys
	./zad1 sort tmp4lib 10000 4 lib
	# 512
	./zad1 generate tmp512 5000 512 lib
	./zad1 copy tmp512 tmp512sys 5000 512 sys
	./zad1 copy tmp512 tmp512lib 5000 512 lib
	./zad1 sort tmp512sys 5000 512 sys
	./zad1 sort tmp512lib 5000 512 lib
	# 1024
	./zad1 generate tmp1024 5000 1024 lib
	./zad1 copy tmp1024 tmp1024sys 5000 1024 sys
	./zad1 copy tmp1024 tmp1024lib 5000 1024 lib
	./zad1 sort tmp1024sys 5000 1024 sys
	./zad1 sort tmp1024lib 5000 1024 lib
	# 4096
	./zad1 generate tmp4096 5000 4096 lib
	./zad1 copy tmp4096 tmp4096sys 5000 4096 sys
	./zad1 copy tmp4096 tmp4096lib 5000 4096 lib
	./zad1 sort tmp4096sys 5000 4096 sys
	./zad1 sort tmp4096lib 5000 4096 lib
	# 8192
	./zad1 generate tmp8192 5000 8192 lib
	./zad1 copy tmp8192 tmp8192sys 5000 8192 sys
	./zad1 copy tmp8192 tmp8192lib 5000 8192 lib
	./zad1 sort tmp8192sys 5000 8192 sys
	./zad1 sort tmp8192lib 5000 8192 lib
clean:
	rm zad1 dane* *tmp*