/*
 * src/unucleo.c
 *
 * unife source.
 * 
 * Germano Andersson <germanoa@gmail.com>
 * Felipe Lahti <felipe.lahti@gmail.com>
 *
 * 2012-05-03
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
    for (i=HIGH+1;i<=LOW;i++) //HIGH+1 because HIGH priority reserved to OS but nos used
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
    for (i=HIGH+1;i<=LOW;i++) //HIGH+1 because HIGH priority reserved to OS but nos used
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
    stats->nr_procs=0;
    stats->nr_parallel_procs=0;

    //Create and alloc mem to join procs map
    if ( (joins = malloc(sizeof(map_join))) == NULL ) { return MALLOCERR; }
    INIT_LIST_HEAD(&joins->next); //init queue for map joins 

    //Zero proc run
    if ( (proc_running = malloc(sizeof(proc_struct))) == NULL ) { return MALLOCERR; }

    return OK;
}

static inline int __in_proc_state(proc_struct *proc, proc_state *state)
{
    int ret = OK;
    proc_state *tmp_state, *tmp;
    tmp_state = state;
    struct list_head  *i;
    list_for_each(i, &tmp_state->lower) {
        tmp = list_entry(i, proc_state, lower);
        if (tmp == NULL) { ret = NULLERR; }
        else if (proc->prio == tmp->prio)
        {
                list_add_tail(&(proc->next), &(tmp->proc_head->next));
        }
    }
    return ret;
}

static inline void __out_proc_state(proc_struct *proc)
{
    list_del(&proc->next);
}

int mproc_create(uint8_t prio, void * (*start_routine)(void*), void * arg)
{
    if (prio < MEDIUM || prio > LOW) { return PRIOERR; }

    if (stats->nr_parallel_procs == MAXPROC) { return MAXPROCERR; }
    
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
    makecontext(&new->context,(void (*)(void))start_routine,1,arg);

    stats->ready_procs++;
    stats->nr_procs++;
    stats->nr_parallel_procs++;

    int ret;
    ret = __in_proc_state(new, ready);
    if (ret == 0) { ret = new->pid; }
    return ret;
}

int mproc_yield(void) {
    int ret;
    ret = __in_proc_state(proc_running, ready);
    if (ret == OK)
    {
        stats->last_proc_state = READY;
        stats->ready_procs++;
        swapcontext(&proc_running->context, &c_sched);
    }
    return ret;
}

static inline int __in_join(uint8_t pid, proc_struct *proc, map_join *joins)
{
    int ret = OK;
    map_join *tmp_join;
    if ( (tmp_join = malloc(sizeof(map_join))) == NULL ) { ret = MALLOCERR; }
    else
    {
        tmp_join->proc = proc;
        tmp_join->pid_joined = pid;
        list_add_tail(&(tmp_join->next), &(joins->next));
    }
    return ret;
}

static inline void __out_join(map_join *join)
{
    list_del(&join->next);
}

static inline int __found_join(uint8_t pid, proc_state *state)
{
    proc_struct *tmp_proc;
    proc_state *tmp_state;
    struct list_head *i,*j;
    list_for_each(i, &state->lower) {
        tmp_state = list_entry(i, proc_state, lower);
        list_for_each(j,&tmp_state->proc_head->next) { //iterator over procs
            tmp_proc = list_entry(j, proc_struct, next);
            if (tmp_proc->pid == pid) { return FOUND; }
        }
    }
    return NOTFOUND;
}


int mproc_join(uint8_t pid)
{
    int ret = OK;
    ret = __in_proc_state(proc_running, blocked); 
    if (ret == OK)
    {
        stats->last_proc_state = BLOCKED;
        __in_join(pid, proc_running, joins);
        stats->blocked_procs++;
        swapcontext(&proc_running->context, &c_sched);
    }
    return ret;
}

static inline void __print_stats(stats_unife *stats)
{
    printf("######################################################\n");
    printf("# SYSTEM STATS\n");
    printf("#\n");
    printf("# Ready process: %d\n",stats->ready_procs);
    printf("# Blocked process: %d\n",stats->blocked_procs);
    printf("# PID last proc running: %d\n",stats->pid_proc_running_now);
    printf("# Proc switches: %d\n",stats->nr_switches_procs);
    printf("# Schedules: %d\n",stats->nr_scheds);
    printf("# Total procs: %d\n",stats->nr_procs);
    printf("# Last PID used: %d\n",stats->last_pid);
    printf("######################################################\n");
}

static inline void __dispatcher(void)
{
    stats->pid_proc_running_now = proc_running->pid;
    stats->nr_switches_procs++;
    stats->last_proc_state = RUNNING;
    swapcontext(&c_sched, &proc_running->context);
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
            list_for_each(j,&tmp_state->proc_head->next) { //iterator over procs
                proc_running = list_entry(j, proc_struct, next);
                __out_proc_state(proc_running);
                stats->ready_procs--;
                __dispatcher();
                if (stats->last_proc_state == RUNNING) { stats->nr_parallel_procs--; }; //to control MAXPROC simultaneous
                new_sched_round=true;
                break; //just the first., we need init sched again
            }
        if (stats->blocked_procs > 0 || stats->ready_procs > 0) {there_are_procs = true; }
        if (new_sched_round) break; //some proc executed, we need init sched again
        }   
    }
    __print_stats(stats);
}
