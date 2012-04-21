#include <stdio.h>
#include <include/unucleo.h>


void *f1()
{
    printf ("f1\n");

    return NULL;
}

void *f2()
{
    printf ("f2\n");

    return NULL;
}


int main (int argc, char *argv[])
{
    int system;
    int i;


    system = libsisop_init();    

    mproc_create(2, f1, &i);    
    mproc_create(1, f1, &i);    

    scheduler();

    return system;
}
