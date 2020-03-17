std: zad1.c
	gcc -g -Wall zad1.c -o zad1

tests:
	make std
	# 1
	./zad1 generate tmp1 50000 1 lib
	./zad1 copy tmp1 tmp1sys 50000 1 sys
	./zad1 copy tmp1 tmp1lib 50000 1 lib
	./zad1 sort tmp1sys 50000 1 sys
	./zad1 sort tmp1lib 50000 1 lib
	rm tmp*
	# 1.2
	./zad1 generate tmp12 200000 1 lib
	./zad1 copy tmp12 tmp12sys 200000 1 sys
	./zad1 copy tmp12 tmp12lib 200000 1 lib
	# ./zad1 sort tmp12sys 200000 1 sys
	# ./zad1 sort tmp12lib 200000 1 lib
	rm tmp*
	# 4
	./zad1 generate tmp4 50000 4 lib
	./zad1 copy tmp4 tmp4sys 50000 4 sys
	./zad1 copy tmp4 tmp4lib 50000 4 lib
	./zad1 sort tmp4sys 50000 4 sys
	./zad1 sort tmp4lib 50000 4 lib
	rm tmp*	
	# 4.2
	./zad1 generate tmp42 200000 4 lib
	./zad1 copy tmp42 tmp42sys 200000 4 sys
	./zad1 copy tmp42 tmp42lib 200000 4 lib
	# ./zad1 sort tmp42sys 200000 4 sys
	# ./zad1 sort tmp42lib 200000 4 lib
	rm tmp*
	# 512
	./zad1 generate tmp512 50000 512 lib
	./zad1 copy tmp512 tmp512sys 50000 512 sys
	./zad1 copy tmp512 tmp512lib 50000 512 lib
	./zad1 sort tmp512sys 50000 512 sys
	./zad1 sort tmp512lib 50000 512 lib
	rm tmp*
	# 512.2
	./zad1 generate tmp5122 200000 512 lib
	./zad1 copy tmp5122 tmp5122sys 200000 512 sys
	./zad1 copy tmp5122 tmp5122lib 200000 512 lib
	# ./zad1 sort tmp5122sys 200000 512 sys
	# ./zad1 sort tmp5122lib 200000 512 lib
	rm tmp*
	# 1024
	./zad1 generate tmp1024 50000 1024 lib
	./zad1 copy tmp1024 tmp1024sys 50000 1024 sys
	./zad1 copy tmp1024 tmp1024lib 50000 1024 lib
	./zad1 sort tmp1024sys 50000 1024 sys
	./zad1 sort tmp1024lib 50000 1024 lib
	rm tmp*
	# 1024.2
	./zad1 generate tmp10242 200000 1024 lib
	./zad1 copy tmp10242 tmp10242sys 200000 1024 sys
	./zad1 copy tmp10242 tmp10242lib 200000 1024 lib
	# ./zad1 sort tmp10242sys 200000 1024 sys
	# ./zad1 sort tmp10242lib 200000 1024 lib
	rm tmp*
	# 4096
	./zad1 generate tmp4096 50000 4096 lib
	./zad1 copy tmp4096 tmp4096sys 50000 4096 sys
	./zad1 copy tmp4096 tmp4096lib 50000 4096 lib
	./zad1 sort tmp4096sys 50000 4096 sys
	./zad1 sort tmp4096lib 50000 4096 lib
	rm tmp*
	# 4096.2
	./zad1 generate tmp40962 200000 4096 lib
	./zad1 copy tmp40962 tmp40962sys 200000 4096 sys
	./zad1 copy tmp40962 tmp40962lib 200000 4096 lib
	# ./zad1 sort tmp40962sys 200000 4096 sys
	# ./zad1 sort tmp40962lib 200000 4096 lib
	rm tmp*
	# 8192
	./zad1 generate tmp8192 50000 8192 lib
	./zad1 copy tmp8192 tmp8192sys 50000 8192 sys
	./zad1 copy tmp8192 tmp8192lib 50000 8192 lib
	./zad1 sort tmp8192sys 50000 8192 sys
	./zad1 sort tmp8192lib 50000 8192 lib
	rm tmp*
	# 8192.2
	./zad1 generate tmp81922 200000 8192 lib
	./zad1 copy tmp81922 tmp81922sys 200000 8192 sys
	./zad1 copy tmp81922 tmp81922lib 200000 8192 lib
	# ./zad1 sort tmp81922sys 200000 8192 sys
	# ./zad1 sort tmp81922lib 200000 8192 lib
	rm tmp*
test:
	make std
	./zad1 generate tmp4 100000 512 lib
	./zad1 copy tmp4 tmp4sys 100000 512 sys
	./zad1 copy tmp4 tmp4lib 100000 512 lib
	./zad1 sort tmp4sys 100000 512 sys
	./zad1 sort tmp4lib 100000 512 lib	
clean:
	rm zad1 dane* *tmp*

