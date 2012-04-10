#include <stdio.h>
#include <include/unucleo.h>


void f1()
{
    printf ("f1\n");
}

void f2()
{
    printf ("f2\n");
}


int main (int argc, char *argv[])
{
    int system;

    system = libsisop_init();    
    printf("system = %d\n",system);
    return system;
}
