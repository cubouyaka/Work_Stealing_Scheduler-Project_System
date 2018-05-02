#include "sched.h"

int sched_init(int nthreads, int qlen, taskfunc f, void *closure){
  if(nthreads == 0) //nombre de threads est choisi par sched_default_threads
    nthreads = sched_default_threads();

  //initialisation du scheduler 
  struct scheduler *scheduler = 
    (struct scheduler *) malloc(sizeof(struct scheduler));
  if(scheduler == NULL){ 
    fprintf(stderr,"Error while creating scheduler [sched_init]\n");
    return -1;
  }

  scheduler->nthreads = nthreads;
  scheduler->qlen = qlen;

  //Creations des threads
  scheduler->threads=(pthread_t *)malloc(scheduler->nthreads *sizeof(pthread_t));

  return 0;
}

int sched_spawn(taskfunc f, void *closure, struct scheduler *s){

  return 0;
}
