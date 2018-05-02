#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

struct scheduler;

typedef void (*taskfunc)(void*, struct scheduler *);


struct scheduler {
  int nthreads; //nombre de threads
  int qlen; //nombres minimum de taches simultanees
  pthread_t * threads ;//tableau de threads
};

static inline int sched_default_threads(){
    return sysconf(_SC_NPROCESSORS_ONLN);
}

int sched_init(int nthreads, int qlen, taskfunc f, void *closure);
int sched_spawn(taskfunc f, void *closure, struct scheduler *s);
