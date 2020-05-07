std: zad1.c
	gcc -g -Wall zad1.c -o zad1.out -lpthread
test:
	for num in 1 2 4 8; do \
	echo ""; echo ""; echo "liczba watkow: $$num"; echo ""; echo "";\
	echo "interleaved:";\
	./zad1.out $$num interleaved balloons_noisy.ascii.pgm out_inter.pgm; \
	echo "block:";\
	./zad1.out $$num block balloons_noisy.ascii.pgm out_block.pgm; \
	echo "sign:";\
	./zad1.out $$num sign balloons_noisy.ascii.pgm out_sign.pgm; \
	done



	#sha1sum out_block.pgm out_inter.pgm out_sign.pgm
clean:
	rm zad1.out