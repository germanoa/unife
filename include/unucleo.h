/*
 * (Header)
 * Unife
 * Thread library in user level N:1 (threads:process)
 * 
 * Universidade Federal do Rio Grande do Sul - UFRGS
 * Sistemas Operacionais I
 *
 * Germano Andersson <germanoa@gmail.com>
 * Felipe Lahti <felipe.lahti@gmail.com>
 *
 * 2012-05-03
 */

#include <include/list.h>
#include <include/retcodes.h>
#include <ucontext.h>
#include <stdint.h>

/*
 * Process Capacity Constants
 */
#define MINPROC 0
#define MAXPROC 128

/*
 * Process State Constants
 */
#define READY 0
#define RUNNING 1
#define BLOCKED 2

/*
 * Process Priority Constants
 */
#define HIGH 0
#define MEDIUM 1
#define LOW 2

/*
 * Context Stack Size
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
    ucontext_t context; /* Store the process context */
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
 * Struct for join maps between process
 */
typedef struct map_join map_join;
struct map_join {
    proc_struct *proc;
    uint8_t pid_joined;
    struct list_head next;
};

/*
 * Statistics Struct.
 */
typedef struct stats_unife stats_unife;
struct stats_unife {
    uint8_t ready_procs;
    uint8_t blocked_procs;
    uint8_t pid_proc_running_now;
    int nr_switches_procs;
    int nr_scheds;
    int last_pid;
    int nr_procs;
    int nr_parallel_procs;
    uint8_t last_proc_state;    
};

/*
 * Init library unife. Must be call before any library call.
 */
int libsisop_init();

/*
 * Create a process. 
 */
int mproc_create(uint8_t prio, void * (*start_routine)(void*), void * arg);

/*
 * Yielding process, nothing interesting to do...
 */
int mproc_yield(void);

/*
 * Synchronized with the end of process pid = pid.
 */
int mproc_join(uint8_t pid);

/*
 * Process scheduler & dispatcher.
 */
void scheduler(void);
