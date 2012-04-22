#include <stdio.h>
#include <include/unucleo.h>


void *f1(void *arg)
{
    printf ("f1\n");
    mproc_yield();
    printf ("fim f1\n");

    return NULL;
}

void *f2(void *arg)
{
    int *i;
    printf ("f2\n");
    mproc_create(1, f1, &i);    
    mproc_yield();
    printf ("fim f2\n");

    return NULL;
}


int main (int argc, char *argv[])
{
    int system;
    int *i;
    //i = 1;


    system = libsisop_init();    

    mproc_create(1, f1, &i);    
    mproc_create(2, f2, &i);    
    mproc_create(2, f2, &i);    
    mproc_create(1, f1, &i);    
    mproc_create(2, f2, &i);    

    scheduler();

    return system;
}
