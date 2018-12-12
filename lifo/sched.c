#include "sched.h"

void empiler(struct Lifo * lifo, taskfunc f, void * closure){
  struct Element * e = (Element *) malloc(sizeof(struct Element));
  
  pthread_mutex_lock (& lifo->mutex);
  if (lifo == NULL)
    exit(EXIT_FAILURE);
  e->t = f;
  e->closure = closure;
  e->prec = lifo->dernier;
  lifo->dernier = e;
  lifo->taille ++;
  pthread_mutex_unlock(& lifo->mutex);
}

Element *depiler(struct Lifo * lifo){
  if (lifo == NULL)
    exit(EXIT_FAILURE);

  Element * e = lifo->dernier; //l'element a retourner

  lifo->dernier = e->prec; //depiler l'element e
  lifo->taille --;
  
  return e;
}

void aux(void * s ){
  struct scheduler *scheduler=(struct  scheduler *)s;

  while(1){
    pthread_mutex_lock (& scheduler->lifo->mutex);
    int t = (scheduler->lifo->taille);
    if(t == 0){ //pile vide
    pthread_mutex_unlock (& scheduler->lifo->mutex);
     
      //incrementer le nombre de thread qui dorment
      pthread_mutex_lock (& scheduler->mutex_sleep);
      scheduler->nbre_th_sleep++;
      pthread_mutex_unlock(& scheduler->mutex_sleep);

      //endors le thread
      pthread_cond_wait (& scheduler->cond, & scheduler->mutex);

      //decrementer le nombre de threads qui dorment
      pthread_mutex_lock (& scheduler->mutex_sleep);
      scheduler->nbre_th_sleep--;
      pthread_mutex_unlock (& scheduler->mutex_sleep);

    }else{ //il y a au moins une tache dans la pile d'execution
      struct Element * e = depiler(scheduler->lifo);
      pthread_mutex_unlock (& scheduler->lifo->mutex);
      e->t(e->closure, scheduler); //execute la tache depiler   
    }
  }
}

int sched_init(int nthreads, int qlen, taskfunc f, void *closure){
  if(nthreads == -1) //nombre de threads est choisi par sched_default_threads
    nthreads = sched_default_threads();

  //initialisation du scheduler
  struct scheduler *scheduler = (struct scheduler *) malloc(sizeof(struct scheduler));
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
  scheduler->lifo->dernier=NULL;
  scheduler->lifo->taille = 0;
  scheduler->lifo->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

  //initialisation du nombre de threads qui dorment
  scheduler->nbre_th_sleep=0;

  //Empilement de la tache initiale sur la pile
  empiler(scheduler->lifo,f,closure);

  //initialisation des mutexs
  scheduler->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
  scheduler->mutex_sleep=(pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

  //la condition d'attente des threeads
  scheduler->cond= (pthread_cond_t)PTHREAD_COND_INITIALIZER;

  int i,t,b,pth;
  fprintf(stderr,"coeur %d \n",nthreads);
  //Lancement des threads
  for (i = 0; i < scheduler->nthreads; i++){
    pth = pthread_create (&scheduler->threads[i], NULL,(void*)aux,
			  (void*)scheduler);
    if (pth != 0)
      fprintf (stderr, "Error dans le thread n-%d [pthread_create]\n", i);
  }

  //retourner lorsque tous les thread sont endormi et la pile vide.
  do{
    usleep(50);
    pthread_mutex_lock (& scheduler->lifo->mutex);
    t = scheduler->lifo->taille;
    pthread_mutex_unlock (& scheduler->lifo->mutex);

    pthread_mutex_lock (& scheduler->mutex);
    b = (scheduler->nbre_th_sleep != scheduler->nthreads);
    pthread_mutex_unlock (& scheduler->mutex);
  }while((t != 0) || b);

  free(scheduler->lifo);
  free(scheduler->threads);
  free(scheduler);
  return 1;
}

int sched_spawn(taskfunc f, void *closure, struct scheduler *s){

  //Si le nombre de tâches en file est supérieur ou égal à la capacité de l’ordonnanceur
  pthread_mutex_lock (& s->lifo->mutex);
  int t = (s->lifo->taille);
  pthread_mutex_unlock (& s->lifo->mutex);
  if(t >= s->qlen){
    errno = EAGAIN;
    return -1;
  }
  // empiler la nouvelle tache
  empiler(s->lifo,f,closure);

  //signal à faire pour reveiller les threads qui dorment
  pthread_cond_signal (& s->cond);
      
  return 0;
}
