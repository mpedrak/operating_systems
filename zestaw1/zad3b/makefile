SHELL=/bin/bash

all:
	make static 
	make shared
	make dynamic

library_static:
	gcc -Wall -c lib.c $(opt)
	ar rc liblib.a lib.o

library_shared:
	gcc -Wall -fPIC -c lib.c $(opt)
	gcc -Wall -shared -fPIC -o liblib.so lib.o $(opt)

static:
	make library_static
	gcc -Wall -c main.c $(opt)
	gcc -Wall main.o liblib.a -o static $(opt)

shared:
	make library_shared
	gcc -Wall -c main.c $(opt)
	gcc -Wall main.o -L. -llib -Wl,-rpath=. -o shared $(opt)

dynamic:
	gcc -Wall -fPIC -c lib.c $(opt)
	gcc -Wall -shared lib.o -o liblib.so $(opt)
	gcc -Wall main2.c -o dynamic -ldl $(opt)

clean:
	rm -f *.o
	rm -f *.so
	rm -f *.a
	rm -f static
	rm -f dynamic
	rm -f shared

testtest:
	make all 
	lines="init 7\ncount main2.c\ncount main.c\ncount lib.c\ncount lib.h\ncount makefile\n"; \
	lines+="show 0\nshow 1\nshow 2\nshow 3\nshow 4\n"; \
	lines+="delete 0\ndelete 0\ndelete 0\ndelete 0\ndelete 0\n"; \
	lines+="destroy\n stop\n"; \
	./static < <(echo -e $$lines) > results_static$(name).txt; \
	./shared < <(echo -e $$lines) > results_shared$(name).txt; \
	./dynamic < <(echo -e $$lines) > results_dynamic$(name).txt
	make clean

test:
	make testtest opt=-O1 name=_O1
	make testtest opt=-O2 name=_O2
	make testtest opt=-O3 name=_O3

cleantxt:
	rm -f results_*.txt 

