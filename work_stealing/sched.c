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

  pthread_mutex_lock (& deque->mutex);

  if(deque->taille == 1){ //si c'est le seul dans la deque
    deque->dernier = NULL;
    deque->premier = NULL;
  }else{
    deque->premier = e->suivant;
    deque->dernier->prec = deque->premier;
  }
  deque->taille --;
  
  pthread_mutex_unlock (& deque->mutex);

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

  pthread_mutex_lock (& deque->mutex);
 
  if(deque->taille == 1){ //si c'est le seul dans la deque
    deque->dernier = NULL;
    deque->premier = NULL;
  }else{
    deque->dernier = e->prec;
    deque->dernier->suivant = deque->premier;
  }
  deque->taille --;
  
  pthread_mutex_unlock (& deque->mutex);

  return e;
}

void aux(void * arg){
  struct arg *args = (struct  arg *)arg;
  int ws = 0;
  while(1){
    if(args->scheduler->mythreads[args->id].deque->taille == 0){ //deque vide
      while(ws == 0){//tant que le work stealing echoue (ie return 0)
	ws = workStealing(args->scheduler,args->id);
	if(ws == 0)
	  usleep(1000); //dors 1ms avant de recommencer une etape de WS
      }	
    }else{ //sinon : la deque n'est pas vide
      pthread_mutex_lock (& args->scheduler->mythreads[args->id].deque->mutex);
      Element * e = defilerBas(args->scheduler->mythreads[args->id].deque);
      pthread_mutex_unlock (& args->scheduler->mythreads[args->id].deque->mutex);

      pthread_mutex_lock(& args->scheduler->mutex);
      args->scheduler->nthreads_sleep --;
      pthread_mutex_unlock(& args->scheduler->mutex);

      e->t(e->closure, args->scheduler); //execute la tache depiler

      pthread_mutex_lock(& args->scheduler->mutex);
      args->scheduler->nthreads_sleep ++;
      pthread_mutex_unlock(& args->scheduler->mutex);
    }
  }
}

int sched_init(int nthreads, int qlen, taskfunc f, void *closure){

  int i,t,pth;

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
  scheduler->nthreads_sleep = nthreads;

  //Allocation de memoire pour les threads
  scheduler->mythreads = (struct MyThread *)
    malloc(scheduler->nthreads*sizeof(struct MyThread));

  //initialisation du mutex
  scheduler->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

  //initialisation des deques
  for (i = 0; i < scheduler->nthreads; i++){
    scheduler->mythreads[i].deque=(struct Deque *)malloc(sizeof(struct Deque));
    scheduler->mythreads[i].deque->premier = NULL;
    scheduler->mythreads[i].deque->dernier = NULL;
    scheduler->mythreads[i].deque->taille = 0;
    scheduler->mythreads[i].deque->mutex = 
      (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    scheduler->mythreads[i].id = i;
  }

  //Enfilement de la tache initiale dans la deque du premier thread
  enfilerHaut(scheduler->mythreads[0].deque,f,closure);

  fprintf(stderr,"coeur %d \n",nthreads);

  //Lancement des threads
  for (i = 0; i < scheduler->nthreads; i++){
    struct arg * args = (struct arg*)malloc(sizeof(struct arg));
    args->scheduler = scheduler;
    args->id = i;
    pth = pthread_create (&scheduler->mythreads[i].thread, NULL,(void*)aux,
			  (void*)args);
    if (pth != 0)
      fprintf (stderr, "Error dans le thread n-%d [pthread_create]\n", i);
  }

  do{
    pthread_mutex_lock (& scheduler->mutex);
    t = scheduler->nthreads - scheduler->nthreads_sleep;
    pthread_mutex_unlock (& scheduler->mutex);
  }while(t != 0); //ie tant qu'il y a des threads reveilles

  //Liberation de la memoire
  for(i = 0; i < scheduler->nthreads; i++)
    free(scheduler->mythreads[i].deque);
  free(scheduler->mythreads);
  free(scheduler);

  return 1;
}

int sched_spawn(taskfunc f, void *closure, struct scheduler *s){

  int id = idMythread(s);
  if(id == -1)
    return -1; //erreur
  
  //enfilement de la tache dans la deque adequate
  pthread_mutex_lock(& s->mythreads[id].deque->mutex);
  enfilerBas(s->mythreads[id].deque,f,closure);
  pthread_mutex_unlock(& s->mythreads[id].deque->mutex);

  return 0;
}

int idMythread(struct scheduler * s){
  int i;
  pthread_t thread = pthread_self();
  for(i = 0; i < s->nthreads; i++)
    if(s->mythreads[i].thread == thread)
      return i;
  return -1; //si il n'a pas trouver (ne devrait jamais arriver)
}

int workStealing(struct scheduler * s, int id){

  int r,i,r_i;
  Element * e;

  srand(time(NULL));
  while(r == id)
    r = rand() % s->nthreads;
  
  pthread_mutex_lock(& s->mythreads[r].deque->mutex);

  if(s->mythreads[r].deque->taille != 0){
    e = defilerHaut(s->mythreads[r].deque);
    e->t(e->closure, s); //execute la tache
    pthread_mutex_unlock(& s->mythreads[r].deque->mutex);
    return 1;
  }
  pthread_mutex_lock(& s->mythreads[r].deque->mutex);
  
  for(i = 1; i < s->nthreads; i++){
    r_i = (r+i)%s->nthreads;
    pthread_mutex_lock(& s->mythreads[r_i].deque->mutex);
    if(s->mythreads[r_i].deque->taille != 0){
      e = defilerHaut(s->mythreads[r_i].deque);
      e->t(e->closure, s); //execute la tache      
      pthread_mutex_unlock(& s->mythreads[r_i].deque->mutex);
      return 1;
    }
    pthread_mutex_unlock(& s->mythreads[r_i].deque->mutex);
  }

  return 0; //echoue
}
