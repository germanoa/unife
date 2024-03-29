/*
 * - Test 1 -
 * Testa inicializacao da lib.
 *
 * - Resultado Esperado:
 * Sistema iniciado com sucesso.
 */

#include <stdio.h>
#include <include/unucleo.h>

int main (int argc, char *argv[])
{
    int system;

    system = libsisop_init();    
    
    if ( system == 0 )
    {
    	printf("Sistema iniciado com sucesso.\n");
    }
    
    scheduler();

    return system;
}
