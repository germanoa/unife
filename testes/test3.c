/*
 * - Test 3 -
 * Testa erro ao atingir o maximo de processos permitidos.
 *
 * - Resultado Esperado:
 * Erro na cricao do processo. Limite de processos excedido.
 */

#include <stdio.h>
#include <include/unucleo.h>

void *f1(void *arg)
{
	int i = 0, x = 0;

	for(i = 0; i < 1000; i++)
	{
		x += i;
		mproc_yield();
	}

    return NULL;
}

int main (int argc, char *argv[])
{
    int system;
    int pid;

    system = libsisop_init();    
    
    int i = 0;
    
    for(i = 0; i < 129; i++)
    {
	    pid = mproc_create(LOW, f1, NULL);
	    
	    if (pid == -1)
	    {
	    	printf("Erro na cricao do processo. Limite de processos excedido.\n");
	    }
    }
    
    scheduler();

    return system;
}
