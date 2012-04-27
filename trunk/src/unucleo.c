/*
 * src/unucleo.c
 *
 * unife source.
 * 
 * Germano Andersson <germanoa@gmail.com>
 * Felipe Lahti <felipe.lahti@gmail.com>
 *
 * 2012-XX-XX
 */


#include <include/unucleo.h>
#include <include/list.h>
#include <include/retcodes.h>

#include <ucontext.h>
#include <stdint.h>
#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>


proc_state *ready, *blocked, *tmp_state, *lower_state;
proc_struct *proc_running;
map_join *joins;
stats_unife *stats;
ucontext_t c_sched;

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
    if ( (joins = malloc(sizeof(map_join))) == NULL ) { return MALLOCERR; }
    INIT_LIST_HEAD(&joins->next); //init queue for map joins 

    //Zero proc run
    if ( (proc_running = malloc(sizeof(proc_struct))) == NULL ) { return MALLOCERR; }

    return OK;
}

int mproc_create(uint8_t prio, void * (*start_routine)(void*), void * arg)
{
    if (prio < MEDIUM || prio > LOW) { return PRIOERR; }

    if (stats->last_pid == MAXPROC) { return MAXPROCERR; }
    
    //Populate PCB
    proc_struct *new;
    if ( (new = malloc(sizeof(proc_struct))) == NULL ) { return MALLOCERR; }
    new->pid = ++stats->last_pid;
    new->state= READY;
    new->prio = prio;

    getcontext(&new->context);
    new->context.uc_stack.ss_sp = malloc(STACK_SIZE);
    new->context.uc_stack.ss_size = STACK_SIZE;
    new->context.uc_link = &c_sched;   
    makecontext(&new->context,(void (*)(void))start_routine,1,arg); //testando

    stats->ready_procs++;

    int ret;
    ret = __in_proc_state(new, ready);
    if (ret == 0) { ret = new->pid; }
    return ret;
}

void mproc_yield(void) {
    __in_proc_state(proc_running, ready);
    stats->ready_procs++;
    swapcontext(&proc_running->context, &c_sched);
}

int mproc_join(uint8_t pid)
{
    int ret = OK;
    ret = __in_proc_state(proc_running, blocked); 
    if (ret == OK)
    {
        __in_join(pid, proc_running, joins);
        stats->blocked_procs++;
        swapcontext(&proc_running->context, &c_sched);
    }
    return ret;
}

void scheduler(void)
{
    proc_state *tmp_state;
    map_join *tmp_join;
    struct list_head  *h, *i,*j; 
    int there_are_procs = true;
    int new_sched_round;

    while (there_are_procs) {
        stats->nr_scheds++;
        there_are_procs = false;

        // Looking for joins to free
        list_for_each(h, &joins->next) {
            tmp_join = list_entry(h,map_join, next);
            // if pid not present at ready and blocked, join can free
            if ( (__found_join(tmp_join->pid_joined, ready) == NOTFOUND) && (__found_join(tmp_join->pid_joined, blocked) == NOTFOUND) )
            {
                __out_join(tmp_join);                            
                __out_proc_state(tmp_join->proc);                            
                __in_proc_state(tmp_join->proc, ready);
                stats->blocked_procs--;
                stats->ready_procs++;
            }
        }

        new_sched_round = false;
        list_for_each(i, &ready->lower) { //iterator over prios of ready state
            tmp_state = list_entry(i, proc_state, lower);
            //printf ("### STATE READY prio:%d\n",tmp->prio);
            list_for_each(j,&tmp_state->proc_head->next) { //iterator over procs
                proc_running = list_entry(j, proc_struct, next);
                //printf ("#PROC pid:%d\n",proc_running->pid);
                __out_proc_state(proc_running);
                stats->ready_procs--;
                //aqui vai o dispatcher
                stats->pid_proc_running_now = proc_running->pid;
                stats->nr_switches_procs++;
                swapcontext(&c_sched, &proc_running->context);
                new_sched_round=true;
                break;
            }
        if (stats->blocked_procs > 0 || stats->ready_procs > 0) {there_are_procs = true; }
        if (new_sched_round) break; //some proc executed, we need init sched again
        }   
    }
    __print_stats(stats);
}
