#include "sched.h"

int main(){

  int nthreads = 3;

  if(nthreads == 0)
    nthreads = sched_default_threads();

  struct scheduler *scheduler = 
    (struct scheduler *) malloc(sizeof(struct scheduler));
  if(scheduler == NULL){
    fprintf(stderr,"Error while creating scheduler [sched_init]\n");
    return -1;
  }

  return 0;
}
