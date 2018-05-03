#include "sched.h"

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

  //Allocation de memoire pour les threads
  scheduler->threads=(pthread_t *)malloc(scheduler->nthreads*sizeof(pthread_t));

  //Creation de la pile d'execution (lifo)
  scheduler->lifo = (struct Lifo *) malloc(sizeof(struct Lifo));

  //Empilement de la tache initiale sur la pile
  empiler(scheduler->lifo,f,closure);

  //Execution de la pile tant qu'il y a des taches a faire
  //Termine lorsque la pile est vide ET tous les threads sont endormis
  struct Element * e = (struct Element *) malloc(sizeof(struct Element));
  int nb_threads_actuel = 0;
  while(scheduler->lifo->dernier != NULL){
    e = depiler(scheduler->lifo);
    //e->t(e->closure,scheduler);
    if(nb_threads_actuel < nthreads){
      /*
      struct arguments* arg =(struct arguments*)malloc(sizeof(struct arguments));
      arg->t = e->t;
      arg->closure = e->closure;
      arg->scheduler = scheduler;
      if(pthread_create(&scheduler->threads[nb_threads_actuel],NULL,&thread_start,&arg) != 0){
	fprintf(stderr,"Erreur lors du pthread_create pour le %d eme thread\n",
		nb_threads_actuel);
      }else{
	nb_threads_actuel ++;
      }
      */
    }
  }

  return 0;
}

/*
static void * thread_start(void *arg){
  arg->t(arg->closure,arg->scheduler);
}
*/

int sched_spawn(taskfunc f, void *closure, struct scheduler *s){

  //Si le nombre de tâches en file est supérieur ou égal à la capacité de l’ordonnanceur
  if(tailleLifo(s->lifo) >= s->qlen){
    errno = EAGAIN;
    return -1;
  }

  //lock à faire
  // empiler la nouvelle tache
  empiler(s->lifo,f,closure);

  //signal à faire pour reveiller les threads qui dorment
  //unlock à faire

  return 0;
}
