install:
	gcc -fPIC -g -c src/unucleo.c -o lib/libsisop.a -I. -Wall
	for i in `seq 1 10`; do gcc -o bin/test$$i.o testes/test$$i.c -I. -Llib -lsisop -Wall -g; done

clean:
	rm -f bin/*.o; rm -f lib/*.a;

