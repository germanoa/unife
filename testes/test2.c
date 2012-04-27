/*
 * testa maxproc = 128
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

int main (int argc, char *argv[])
{
    int system;
    int x,i;
    x = 7;

    system = libsisop_init();    
    
    for (i=0;i<=MAXPROC*2;i++)
    {
        mproc_create(1, f1, &x);    
    }

    scheduler();

    return system;
}
