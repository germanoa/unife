/*
 * src/unucleo.c
 *
 * unife source.
 * 
 * TODO - vamos dividir em mais de um modulo?
 *
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

proc_state *procs_ready, *procs_blocked, *procs_tmp;
proc_struct *proc_running;
map_join *joins;
stats_unife *stats;

int libsisop_init()
{
    int i;
    // Create and alloc mem to proc state ready list with prio
    if ( (procs_ready = malloc(MAXPROC * sizeof(proc_state))) == NULL ) {return MALLOCERR; }
    procs_tmp = procs_ready;
    for (i=HIGH;i<=LOW;i++)
    {
        procs_tmp->prio = i;
        procs_tmp->proc = NULL;
        procs_tmp->next = NULL;
        if ( (procs_tmp->lower_prio = malloc(MAXPROC * sizeof(proc_state))) == NULL ) {return MALLOCERR; }
        procs_tmp = procs_tmp->lower_prio; 
    }

    // Create and alloc mem to proc state blocked list with prio
    if ( (procs_blocked = malloc(MAXPROC * sizeof(proc_state))) == NULL ) { return MALLOCERR; }
    procs_tmp = procs_blocked;
    for (i=HIGH;i<=LOW;i++)
    {
        procs_tmp->prio = i;
        procs_tmp->proc = NULL;
        procs_tmp->next = NULL;
        if ( (procs_tmp->lower_prio = malloc(MAXPROC * sizeof(proc_state))) == NULL ) {return MALLOCERR; }
        procs_tmp = procs_tmp->lower_prio; 
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
    proc_running = NULL;

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
    //new.context = NULL;

    int ret;
    ret = new->pid;
    if (! __in_proc_state(new, procs_ready)) { ret = -1; }

    return ret;
}


void scheduler(void)
{
    if (proc_running != NULL) //Not first round
    {
        printf("verificar se ha join\n");
        // se existe algum proc blocked joined ao proc_running
        //      enviar proc blocked para ready & proc_running = NULL.
        //      (fazer para todos processos em blocked, pois pode haver mais de um joined ao proc q terminou).
    }

    int there_are_procs = 1;

    procs_tmp = procs_ready;
    while ( there_are_procs )
    {
        if (procs_tmp->proc != NULL)
        {
            proc_running = procs_tmp->proc;
            procs_tmp->proc = NULL; // teste, depois tem q usar funcao deleta lista
            //chavear contexto & break(fazer com que no retorno do swapcontext retorne ao inicio da funcao)
            
            // por aqui tem o dispatcher. ele é um simples chaveamento de contexto?
            printf("PID: %d em prio %d\n",proc_running->pid, procs_tmp->prio);

        }
        else if (procs_tmp->lower_prio != NULL)
        { 
            procs_tmp = procs_tmp->lower_prio;    
        }
        else there_are_procs = 0;
    }
}
