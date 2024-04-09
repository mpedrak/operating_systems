SHELL=/bin/bash

all:
	gcc -Wall main.c -o main

clean:
	rm main

test:
	make all 
	./main 0.000000001 1 > raport2.txt; 
	./main 0.000000001 2 >> raport2.txt; 
	./main 0.000000001 3 >> raport2.txt; 
	./main 0.000000001 4 >> raport2.txt; 
	./main 0.000000001 5 >> raport2.txt; 
	./main 0.000000001 6 >> raport2.txt; 
	./main 0.000000001 7 >> raport2.txt; 
	./main 0.000000001 10 >> raport2.txt; 
	./main 0.000000001 20 >> raport2.txt; 
 
	./main 0.0000000001 4 >> raport2.txt; 
	./main 0.0000000001 20 >> raport2.txt; 
	make clean