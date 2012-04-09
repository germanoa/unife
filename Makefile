install:
	gcc -fPIC -c src/unucleo.c -o lib/libsisop.a -I. -Wall ; gcc -o bin/test1.o testes/test1.c -I. -Llib -lsisop -Wall

clean:
	rm -f bin/*.o; rm -f lib/*.a;

