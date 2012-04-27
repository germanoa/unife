/*
 * include/unucleo.h
 *
 * headers do unife.
 * 
 * internal functions (__xxx) must not be used directly.
 * 
 * Germano Andersson <germanoa@gmail.com>
 * Felipe Lahti <felipe.lahti@gmail.com>
 *
 * 2012-XX-XX
 */

#include <include/list.h>
#include <include/errcodes.h>

#include <ucontext.h>
#include <stdint.h>
#include <stdio.h>
#include <malloc.h>


/*
 * process capacity
 */
#define MINPROC 0
#define MAXPROC 128

/*
 * process state
 */
#define READY 0
#define RUNNING 1
#define BLOCKED 2

/*
 * process prio
 */
#define HIGH 0
#define MEDIUM 1
#define LOW 2


/*
 * context
 */
#define STACK_SIZE 1024*64


/*
 * PCB (Process Control Block)
 */
typedef struct proc_struct proc_struct;
struct proc_struct {
    uint8_t pid;
    uint8_t state;  /* 0-ready  1-running   2-blocked */
    uint8_t prio;   /* 0-high(only unife) 1-medium 2-low */
    ucontext_t context;
    struct list_head next;
};

/*
 * Proc State 
 */
typedef struct proc_state proc_state;
struct proc_state {
    uint8_t prio;   /* 0-high(only unife) 1-medium 2-low */
    proc_struct *proc_head; /* The first proc in this state+prio */
    struct list_head lower; /* The lower prio of the same state */
};


/*
 * Creating process. 
 */
int mproc_create(uint8_t prio, void * (*start_routine)(void*), void * arg);

/*
 * Yielding process, nothing interesting to do...
 */
void mproc_yield(void);

/*
 * Synchronized with the end of process pid = pid.
 */
int mproc_join(uint8_t pid);

typedef struct map_join map_join;
struct map_join {
    proc_struct *proc;
    uint8_t pid_joined;
    struct list_head next;
};

static inline int __in_join(uint8_t pid, proc_struct *proc, map_join *joins)
{
    int ret = 1;
    map_join *tmp_join;
    if ( (tmp_join = malloc(sizeof(map_join))) == NULL ) { return MALLOCERR; }
    else
    {
        tmp_join->proc = proc;
        tmp_join->pid_joined = pid;
        list_add_tail(&(tmp_join->next), &(joins->next));
    }
    return ret;
}

static inline int __out_join(map_join *join)
{
    list_del(&join->next);
    return 1;
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
            if (tmp_proc->pid == pid)
            {
                //printf ("encontrei pid %d no state %p\n",pid,tmp_state);
                return 1;
            }    
        }
    }
    return 0;
}


static inline int __in_proc_state(proc_struct *proc, proc_state *state)
{
    int ret = 1;
    proc_state *tmp_state, *tmp;
    tmp_state = state;
    struct list_head  *i;
    list_for_each(i, &tmp_state->lower) {
        tmp = list_entry(i, proc_state, lower);
        if (tmp == NULL) { ret = -1; }
        else if (proc->prio == tmp->prio)
        {
                list_add_tail(&(proc->next), &(tmp->proc_head->next));   
        }
    }
    return ret;
}

static inline int __out_proc_state(proc_struct *proc)
{
    list_del(&proc->next);
    return 1;
}

/*
 * Init unife.
 */
int libsisop_init();

/*
 * Scheduler & dispatcher.
 */
void scheduler(void);

/*
 * Statistics.
 */
typedef struct stats_unife stats_unife;
struct stats_unife {
    uint8_t ready_procs;
    uint8_t blocked_procs;
    uint8_t pid_proc_running_now;
    int nr_switches_procs;
    int nr_scheds;
    uint8_t last_pid;
};

static inline void __print_stats(stats_unife *stats)
{
    printf("######################################################\n");
    printf("# Ready process: %d\n",stats->ready_procs);
    printf("# Blocked process: %d\n",stats->blocked_procs);
    printf("# PID last proc running: %d\n",stats->pid_proc_running_now);
    printf("# Proc switches: %d\n",stats->nr_switches_procs);
    printf("# Schedules: %d\n",stats->nr_scheds);
    printf("# Last PID used: %d\n",stats->last_pid);
    printf("######################################################\n");
}
