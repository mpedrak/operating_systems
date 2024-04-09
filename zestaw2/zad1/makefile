SHELL=/bin/bash

all:
	gcc -Wall bezef.c -o bezef
	gcc -Wall zef.c -o zef

clean:
	rm bezef
	rm zef
	rm t1 
	rm t2

test:
	make all
	./bezef i 7 text t1 > pomiar_zad_1.txt
	tr i 7 < text > t2
	diff -s t1 t2 >> pomiar_zad_1.txt
	./zef i 7 text t1 >> pomiar_zad_1.txt
	diff -s t1 t2 >> pomiar_zad_1.txt
	make clean