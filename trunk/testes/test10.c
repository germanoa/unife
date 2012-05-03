/*
 * - Test 10 -
 * Find a random number by adding or subtracting a number. Try to do in minimal steps as possible.
 *
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <include/unucleo.h>

int NUMBER_TO_FOUND;

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
			printf("Parabens voce acertou o numero em %d passos!\n", passos);
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
    srand (time (NULL));
    NUMBER_TO_FOUND = rand() % 10000;
    
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
