/*
 * - Test 11 -
 * Programa oi Or tchau.
 */

#include <stdio.h>
#include <string.h>
#include <include/unucleo.h>

void *retruco(void *arg)
{
    int *resposta;
    resposta = arg;
    switch ((int)*resposta)
    {
        case 1:
            printf ("oi\n");
            break;
        case 2:
            printf ("tchau\n");
            break;
        default:
            printf("opcao invalida\n");
    }
    return NULL;
}


void *truco(void *arg)
{
    int *pfala;
    pfala = arg;
    printf ("Digite 1 para 'oi' ou 2 para 'tchau':\n");
    scanf("%d",pfala);
    mproc_create(HIGH+1,retruco,pfala);
    return NULL;
}

int main (int argc, char *argv[])
{
    int system;
    int fala;

    system = libsisop_init();    
    
    mproc_create(LOW, truco, &fala);    

    scheduler();

    return system;
}
