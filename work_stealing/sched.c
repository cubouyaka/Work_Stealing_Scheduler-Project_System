#include "sched.h"

//Enfile la taskfunc f en haut de la deque 
void enfilerHaut(struct Deque * deque, taskfunc f, void *closure){
  struct Element * e = (Element *) malloc(sizeof(struct Element));

  pthread_mutex_lock (& deque->mutex);
  if (deque == NULL)
    exit(EXIT_FAILURE);

  e->t = f;
  e->closure = closure;
  e->prec = deque->dernier;
  e->suivant = deque->premier;
  deque->premier = e;
  if(deque->taille == 0) //si la deque etait vide avant
    deque->dernier = e;
  deque->taille ++;
  pthread_mutex_unlock (& deque->mutex);
}

//Defile la deque par en haut
Element* defilerHaut(struct Deque * deque){
  if (deque == NULL)
    exit(EXIT_FAILURE);

  Element * e = deque->premier; //l'element a retourner

  if(deque->taille == 1){ //si c'est le seul dans la deque
    deque->dernier = NULL;
    deque->premier = NULL;
  }else{
    deque->premier = e->suivant;
    deque->dernier->prec = deque->premier;
  }
  deque->taille --;
  
  return e;

}

//Enfile la taskfunc f en bas de la deque 
void enfilerBas(struct Deque * deque, taskfunc f, void *closure){
  struct Element * e = (Element *) malloc(sizeof(struct Element));

  pthread_mutex_lock (& deque->mutex);
  if (deque == NULL)
    exit(EXIT_FAILURE);

  e->t = f;
  e->closure = closure;
  e->prec = deque->dernier;
  e->suivant = deque->premier;
  deque->dernier = e;
  if(deque->taille == 0) //si la deque etait vide avant
    deque->premier = e;
  deque->taille ++;
  pthread_mutex_unlock (& deque->mutex);
}

//Defile la deque par en bas
Element* defilerBas(struct Deque * deque){
  if (deque == NULL)
    exit(EXIT_FAILURE);

  Element * e = deque->dernier; //l'element a retourner

  if(deque->taille == 1){ //si c'est le seul dans la deque
    deque->dernier = NULL;
    deque->premier = NULL;
  }else{
    deque->dernier = e->prec;
    deque->dernier->suivant = deque->premier;
  }
  deque->taille --;
  
  return e;
}

int sched_init(int nthreads, int qlen, taskfunc f, void *closure){

  int p;

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
  scheduler->mythreads = (struct MyThread *)
    malloc(scheduler->nthreads*sizeof(struct MyThread));

  //initialisation du mutex
  scheduler->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

  //initialisation des deques
  for (p = 0; p < scheduler->nthreads; p++){
    scheduler->mythreads[p].deque=(struct Deque *)malloc(sizeof(struct Deque));
    scheduler->mythreads[p].deque->premier = NULL;
    scheduler->mythreads[p].deque->dernier = NULL;
    scheduler->mythreads[p].deque->taille = 0;
    scheduler->mythreads[p].deque->mutex = 
      (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
  }

  //Enfilement de la tache initiale dans la deque du premier thread
  enfilerHaut(scheduler->mythreads[0].deque,f,closure);

  /* A CONTINUER ICI

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

  */

  return 1;
}

int sched_spawn(taskfunc f, void *closure, struct scheduler *s){

  /*

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
      

  */

  return 0;
}
