/*
 * - Test 7 -
 * Teste prioridades 2. Yield apenas para processos de prioridade superior ou igual.
 */

#include <stdio.h>
#include <include/unucleo.h>


void *f1(void *arg)
{
    int *z;
    z = arg;
    printf ("f1 x=%d\n",(int)*z);
    mproc_yield();
    printf ("fim f1\n");

    return NULL;
}

void *f2(void *arg)
{
    int *p;
    p = arg;
    printf ("f2 p=%d\n",(int)*p);
    int i;
    i = 5;
    mproc_create(1, f1, &i);    
    mproc_yield();
    mproc_join(1);
    printf ("fim f2\n");
    return NULL;
}

int main (int argc, char *argv[])
{
    int system;
    int x;
    x = 7;
    int y;
    y = 10;

    system = libsisop_init();    
    
    mproc_create(1, f1, &x);    
    mproc_create(2, f2, &y);    
    mproc_create(1, f1, &x);    
    mproc_create(1, f1, &x);    
    mproc_create(2, f2, &y);    
    mproc_create(2, f2, &y);    

    scheduler();

    return system;
}
