#include "sched.h"

int sched_init(int nthreads, int qlen, taskfunc f, void *closure){
  if(nthreads == 0)
    nthreads = sched_default_threads();

  struct scheduler *scheduler = 
    (struct scheduler *) malloc(sizeof(struct scheduler));
  if(scheduler == NULL){
    fprintf(stderr,"Error while creating scheduler [sched_init]\n");
    return -1;
  }

  scheduler->nthreads = nthreads;
  scheduler->qlen = qlen;

}

int sched_spawn(taskfunc f, void *closure, struct scheduler *s){


}
