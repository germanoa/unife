/*
 * src/unucleo.c
 *
 * unife source.
 * 
 * TODO - vamos dividir em mais de um modulo?
 *      - qual o tamanho ideal para alocarmos memoria aos proc_state? por enquanto MAXPROC, mas nao faz mto sentido
 * Germano Andersson <germanoa@gmail.com>
 * Felipe Lahti <felipe.lahti@gmail.com>
 *
 * 2012-XX-XX
 */

#include <ucontext.h>
#include <stdint.h>
#include <include/unucleo.h>
#include <include/list.h>
#include <include/errcodes.h>
#include <stdio.h>
#include <malloc.h>

proc_state *ready, *blocked, *tmp_state, *lower_state;
proc_struct *proc_running;
map_join *joins;
stats_unife *stats;

int libsisop_init()
{
    int i;
    // Create and alloc mem to proc state ready list with prio
    if ( (ready = malloc(sizeof(proc_state))) == NULL ) {return MALLOCERR; }
    INIT_LIST_HEAD(&ready->lower); // init queue for state ready
    tmp_state = ready;
    for (i=HIGH;i<=LOW;i++)
    {
        if ( (lower_state = malloc(sizeof(proc_state))) == NULL ) {return MALLOCERR; }
        list_add(&(lower_state->lower), &(tmp_state->lower));
        tmp_state = lower_state;
        tmp_state->prio = i;
        if ( (tmp_state->proc_head = malloc(sizeof(proc_struct))) == NULL ) { return MALLOCERR; }
        INIT_LIST_HEAD(&tmp_state->proc_head->next); //init queue for this state+prio
    }

    // Create and alloc mem to proc state blocked list with prio
    if ( (blocked = malloc(sizeof(proc_state))) == NULL ) { return MALLOCERR; }
    INIT_LIST_HEAD(&blocked->lower); //init queue for state blocked
    tmp_state = blocked;
    for (i=HIGH;i<=LOW;i++)
    {
        if ( (lower_state = malloc(sizeof(proc_state))) == NULL ) {return MALLOCERR; }
        list_add(&(lower_state->lower), &(tmp_state->lower));
        tmp_state = lower_state;
        tmp_state->prio = i;
        if ( (tmp_state->proc_head = malloc(sizeof(proc_struct))) == NULL ) { return MALLOCERR; }
        INIT_LIST_HEAD(&tmp_state->proc_head->next); //init queue for this state+prio
    }

    // Create and alloc mem to statistics/contability of the system
    if ( (stats = malloc(sizeof(stats_unife))) == NULL ) { return MALLOCERR; }
    stats->ready_procs = 0;
    stats->blocked_procs = 0;
    stats->pid_proc_running_now = 0;
    stats->nr_switches_procs = 0;
    stats->nr_scheds=0;
    stats->last_pid=MINPROC;

    //Create and alloc mem to join procs map
    if ( (joins = malloc(MAXPROC * MAXPROC *  sizeof(map_join))) == NULL ) { return MALLOCERR; }

    //Zero proc run
    if ( (proc_running = malloc(sizeof(proc_struct))) == NULL ) { return MALLOCERR; }

    return 0;
}

int mproc_create(uint8_t prio, void * (*start_routine)(void*), void * arg)
{
    if (prio < MEDIUM || prio > LOW) { return PRIOERR; }
    
    //Populate PCB
    proc_struct *new;
    if ( (new = malloc(sizeof(proc_struct))) == NULL ) { return MALLOCERR; }
    new->pid = ++stats->last_pid;
    new->state= READY;
    new->prio = prio;

    /*
    char stack[STACK_SIZE]; //nao deveriamos alocar memoria para este stack?
    getcontext(&new->context);
    new->context.uc_stack.ss_sp = stack;
    new->context.uc_stack.ss_size = sizeof stack;
    new->context.uc_link = NULL;   
    makecontext(&new->context,start_routine,0); //testando
    */

    int ret;
    ret = new->pid;
    if (! __in_proc_state(new, ready)) { ret = -1; }
    return ret;
}

void scheduler(void)
{
    /*
    if (nao eh primeira rodada, verificar mapa de joins)
    {
        // se existe algum proc blocked joined ao proc_running
        //      enviar proc blocked para ready & proc_running = NULL.
        //      (fazer para todos processos em blocked, pois pode haver mais de um joined ao proc q terminou).
    }
    */

    proc_state *tmp_state, *tmp;
    tmp_state = ready;
    struct list_head  *i,*j; 
    list_for_each(i, &tmp_state->lower) { //iterator over prios of ready state
        tmp = list_entry(i, proc_state, lower);
        list_for_each(j,&tmp->proc_head->next) { //iterator over procs
            proc_running = list_entry(j, proc_struct, next);
            printf ("proc:%p; pid:%d, prio:%d\n",proc_running, proc_running->pid, proc_running->prio);
            __out_proc_state(proc_running);
            //aqui vai o dispatcher
            //if (tmp->proc_head != NULL) { printf ("proc:%p; pid:%d, prio:%d\n",proc_running, proc_running->pid, proc_running->prio); }
        }
    }   
}
