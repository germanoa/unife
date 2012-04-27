/*
 * deadlock join * join
 */

#include <stdio.h>
#include <include/unucleo.h>


void *f1(void *arg)
{
    int *pid_join;
    pid_join = arg;
    printf ("Vou aguardar finalizacao pid=%d\n",(int)*pid_join);
    mproc_join((int)*pid_join);
    printf ("fim f1\n");

    return NULL;
}

int main (int argc, char *argv[])
{
    int system;
    int pid1,pid2;

    system = libsisop_init();    
    
    pid1 = mproc_create(1, f1, &pid2);    
    pid2 = mproc_create(2, f1, &pid1);    

    scheduler();

    return system;
}
