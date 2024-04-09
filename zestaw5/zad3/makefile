SHELL=/bin/bash

all:
	gcc -Wall main.c -o main
	gcc -Wall licz.c -o licz

clean:
	rm licz
	rm main

test:
	make all 
	./main 0.000000001 1 > raport3.txt; 
	./main 0.000000001 2 >> raport3.txt; 
	./main 0.000000001 3 >> raport3.txt; 
	./main 0.000000001 4 >> raport3.txt; 
	./main 0.000000001 5 >> raport3.txt; 
	./main 0.000000001 6 >> raport3.txt; 
	./main 0.000000001 7 >> raport3.txt; 
	./main 0.000000001 10 >> raport3.txt; 
	./main 0.000000001 20 >> raport3.txt; 
 
	./main 0.0000000001 4 >> raport3.txt; 
	./main 0.0000000001 20 >> raport3.txt; 
	make clean