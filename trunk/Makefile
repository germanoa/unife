clean:
	rm -f bin/*.o; rm -f *.o; rm -f lib/*.a;

install:
	gcc -fPIC -g -c src/unucleo.c -o lib/libsisop.a -I. -Wall
	for i in `ls -1 testes/ |cut -d'.' -f1`; do gcc -o $$i.o testes/$$i.c -I. -Llib -lsisop -Wall -g; done
