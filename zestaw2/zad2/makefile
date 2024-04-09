SHELL=/bin/bash

all:
	gcc -Wall single.c -o single
	gcc -Wall blocks.c -o blocks

clean:
	rm single
	rm blocks
	rm t1 
	rm t2

test:
	make all
	./single text t1 > pomiar_zad_2.txt
	./single t1 t2
	diff -s text t2 >> pomiar_zad_2.txt
	./blocks text t1 >> pomiar_zad_2.txt
	./blocks t1 t2 
	diff -s text t2 >> pomiar_zad_2.txt
	make clean