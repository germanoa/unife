/*
 * include/unucleo.h
 *
 * headers do unife.
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
 * PCB (Process Control Block)
 */
typedef struct proc_struct proc_struct;
struct proc_struct {
    uint8_t pid;
    uint8_t state;  /* 0-ready  1-running   2-blocked */
    uint8_t prio;   /* 0-high 1-medium 2-low */
    ucontext_t context;
};

/*
 * Proc State Ready list
 */
struct proc_state_ready {
    proc_struct *proc;
    proc_struct *next;
};

/*
 * Proc State Blocked list
 */
struct proc_state_blocked {
    proc_struct *proc;
    proc_struct *next;
};

/*
 * Creating process. 
 */
int mproc_create(int prio, void * (*start_routine)(void*), void * arg);

/*
 * Yielding process, nothing interesting to do...
 */
void mproc_yield(void);

/*
 * Synchronized with the end process pid.
 */
int mproc_join(int pid);

/*
 * Changing state, the queue goes!
 */
int change_proc_state(int pid);

/* "local" functions :p */
static inline int add_proc_state_ready(int pid);
static inline int del_proc_state_ready(int pid);
static inline int add_proc_state_blocked(int pid);
static inline int del_proc_state_blocked(int pid);

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
struct stats_unife {
    uint8_t ready_procs;
    uint8_t blocked_procs;
    uint8_t pid_proc_running_now;
    int nr_switches_procs;
    int nr_scheds;
};
