/*
 * - Test 5 -
 * Teste prioridades
 * Execucao:
 * - Cria processo f1 com prioridade LOW e f2 com prioridade MEDIUM.
 * - Executa processo F2 ate o fim pois este tem prioridade maior.
 * - Processo F1 inicia execucao e cria um processo com prioridade maior que a sua ( MEDIUM ).
 * - Processo F1 para de executar apos seu primeiro yield. F2 executa ate o fim, pois tem prioridade maior.
 * - Processo f2 e finalizado.
 * - Processo f1 e finalizado.
 *
 * - Result Expected:
 * Start F2
 * End F2
 * Start F1
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
  	int i = 0, x;
  	mproc_create(MEDIUM, f2, NULL);
  	for (i = 0; i < 10000; i++)
	{
		x += i;
		mproc_yield();
	}

    printf ("End F1\n");

    return NULL;
}

int main (int argc, char *argv[])
{
    int system;

    system = libsisop_init();    
    
    mproc_create(LOW, f1, NULL);  
    mproc_create(MEDIUM, f2, NULL); 
        
    scheduler();

    return system;
}
