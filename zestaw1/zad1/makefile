SHELL=/bin/bash

all:
	make library_shared
	make library_static
	
library_static:
	gcc -Wall -c lib.c $(opt)
	ar rc liblib.a lib.o

library_shared:
	gcc -Wall -fPIC -c lib.c $(opt)
	gcc -Wall -shared -fPIC -o liblib.so lib.o $(opt)

clean:
	rm -f *.o
	rm -f *.so
	rm -f *.a
	rm -f static
	rm -f dynamic
	rm -f shared
