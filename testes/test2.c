/*
 * - Test 2 -
 * Teste erro cricao de processo com prioridade diferente da permitida.
 *
 * - Resultado Esperado:
 * Erro na cricao do processo. Prioridade nao permitida.\
 */

#include <stdio.h>
#include <include/unucleo.h>

void *f1(void *arg)
{
	printf("Start F1\n");
	mproc_yield();
    printf ("End F1\n");

    return NULL;
}

int main (int argc, char *argv[])
{
    int system;
    int pid1;

    system = libsisop_init();    
    
    pid1 = mproc_create(99, f1, NULL);  
	
	if ( pid1 == -1 )
	{
		printf("Erro na cricao do processo. Prioridade nao permitida.\n");
	}
        
    scheduler();

    return system;
}
