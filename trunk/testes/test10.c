/*
 * - Test 9 -
 * Testa validacao de processos maximos simultaneos. Podem existir n durante execucao.
 *
 * - Resultado Esperado:
 * Sistema criará 10000 processos
 */

#include <stdio.h>
#include <include/unucleo.h>

#define NUMBER_TO_FOUND 500

void *update(void *arg)
{
	int incremment = 0;
	int *number;	

	number = arg;

	while(1)
	{	
		printf("Acrescentar ao numero inicial o valor:\n"); 
		scanf("%d", &incremment);	
		*number += incremment;

		mproc_yield();
		
		if ( *number == NUMBER_TO_FOUND) break;
	}
	return NULL;
}

void *printNumber(void *arg)
{
	int local_number = 0, *number;	
	number = arg;
	int passos = 0;
	
	mproc_yield();
	while ( 1 )
	{
		if (local_number != *number)
		{
			local_number = (int)*number;
			printf("Numero atualizado: %d\n", local_number);			
			passos++;
		}
		
		if ( (int) *number == NUMBER_TO_FOUND )
		{
			printf("Parabens voce acertou o numero em %d\n", passos);
			break;
		}
		
		if ( (int) *number < NUMBER_TO_FOUND )
		{
			printf("Numero e maior! Aumente mais!\n");
		}
		
		if ( (int) *number > NUMBER_TO_FOUND )
		{
			printf("Numero e menor! Reduza mais!\n");
		}
		
		mproc_yield();
	}	
	
	return NULL;
}

int main (int argc, char *argv[])
{
    int system;
    int pid;
    int number;
    number = 0;

    system = libsisop_init();    
    
    printf("Descubra o numero magico no menor numero de passos somando ou subtraindo um valor inteiro:\n");
    pid = mproc_create(MEDIUM, printNumber, &number);
    pid = mproc_create(MEDIUM, update, &number);
           
    if (pid == -1)
    {
    	printf("Erro na cricao do processo. Limite de processos excedido.\n");
    }
    
    scheduler();

    return system;
}
