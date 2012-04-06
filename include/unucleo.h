

struct proc_struct {
    int pid;
    int state;  /* 0-ready  1-running   2-blocked */
    int prio;   /* 0-alta 1-media 2-baixa */
    int context;
};


int libsisop_init();
int mproc_create(int prio, void * (*start_routine)(void*), void * arg);
void mproc_yield(void);
int mproc_join(int pid);
