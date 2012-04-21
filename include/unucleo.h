/*
 * include/unucleo.h
 *
 * headers do unife.
 * 
 * internal functions (__xxx) must not be used directly.
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
#include <stdio.h>

#include <include/list.h>

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
#define STACK_SIZE 16384


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
    uint8_t pid;
    uint8_t pid_joined;
};

static inline int __in_proc_state(proc_struct *proc, proc_state *state)
{
    proc_state *tmp_state, *tmp;
    tmp_state = state;
    struct list_head  *i;
    list_for_each(i, &tmp_state->lower) {
        tmp = list_entry(i, proc_state, lower);
        if (proc->prio == tmp->prio)
        {
                list_add_tail(&(proc->next), &(tmp->proc_head->next));   
        }
    }
    return 1;
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
