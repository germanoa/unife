/*
 * - Test 5 -
 * Teste funcao mproc_join(pid).
 * Execucao:
 * - Cria processo f1.
 * - Processo f1 cria processo f2.
 * - Processo f1 aguarda finalizacao do processo f2 atraves da chamada mproc_join(pid_f2).
 * - Processo f2 e finalizado.
 * - Processo f1 e finalizado.
 *
 * - Result Expected:
 * Start F1
 * Waiting for F2
 * Start F2
 * End F2
 * End F1
 */

#include <stdio.h>
#include <include/unucleo.h>

void *f2(void *arg)
{
	int x = 0, i;
	
	printf("Start F2\n");
	
	//Expensive task
	for(i= 0; i < 1000000; ++i)
	{
		x = x + i;
		mproc_yield();
	}
	
	printf("End F2\n");

    return NULL;
}

void *f1(void *arg)
{
	printf("Start F1\n");
   
    int pid2 = mproc_create(2, f2, NULL);    
    printf ("Waiting for F2\n");
    mproc_join(pid2);

    printf ("End F1\n");

    return NULL;
}

int main (int argc, char *argv[])
{
    int system;

    system = libsisop_init();    
    
    mproc_create(1, f1, NULL);  
    
    scheduler();

    return system;
}
