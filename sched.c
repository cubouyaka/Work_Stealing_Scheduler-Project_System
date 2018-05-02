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

  //Creation de la pile d'execution (lifo)
  scheduler->lifo = (struct Lifo *) malloc(sizeof(struct Lifo));

  //Empilation de la tache initiale sur la pile
  empiler(scheduler->lifo,f,closure);

  return 0;
}

int sched_spawn(taskfunc f, void *closure, struct scheduler *s){

  return 0;
}


void empiler(struct Lifo * lifo, taskfunc f, void * closure){
  struct Element * e = (Element *) malloc(sizeof(struct Element));
  if (lifo == NULL || e == NULL)
    exit(EXIT_FAILURE);
  e->t = f;
  e->closure = closure;
  e->prec = lifo->dernier;
  lifo->dernier = e;
}

Element *depiler(struct Lifo * lifo){
    if (lifo == NULL)
      exit(EXIT_FAILURE);

    Element * e = lifo->dernier; //l'element a retourner
    lifo->dernier = e->prec; //depiler l'element e

    return e;
}

int tailleLifo(struct Lifo * lifo){
  int c = 0;
  struct Element * e = lifo->dernier;
  while(e != NULL){
    c++;
    e = e->prec;
  }
  return c;
}
