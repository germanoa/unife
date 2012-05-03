/*
 * - Test 9 -
 * Testa validacao de processos maximos simultaneos. Podem existir n durante execucao.
 *
 * - Resultado Esperado:
 * Sistema criará 10000 processos
 */

#include <stdio.h>
#include <include/unucleo.h>


void *f1(void *arg)
{
    int pid,*pi;
    pi = arg;
    *pi = (int)*pi+1;
    printf ("f1 i=%d \n",(int)*pi);
    if ( (int)*pi < 10000 ) {	pid = mproc_create(LOW, f1, pi); }
    mproc_yield();
    printf ("end f1\n");
    return NULL;
}

int main (int argc, char *argv[])
{
    int system;
    int pid;
    int i;
    i = 0;

    system = libsisop_init();    
    
    pid = mproc_create(LOW, f1, &i);
    
    if (pid == -1)
    {
    	printf("Erro na cricao do processo. Limite de processos excedido.\n");
    }
    
    scheduler();

    return system;
}
