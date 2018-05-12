#include "sched.h"

//Enfile la taskfunc f en haut de la deque 
void enfilerHaut(struct Deque * deque, taskfunc f, void *closure){
  struct Element * e = (Element *) malloc(sizeof(struct Element));

  pthread_mutex_lock (& deque->mutex);
  if (deque == NULL)
    exit(EXIT_FAILURE);

  e->t = f;
  e->closure = closure;
  e->prec = NULL;

  if(deque->taille != 0){
    deque->premier->prec = e;
    e->suivant = deque->premier;
    deque->premier = e;
  }else{
    e->suivant = NULL;
    deque->premier = e;
    deque->dernier = e;
  }
  deque->taille ++;
  
  /*
  //e->prec = deque->dernier;
  e->prec = NULL;
  e->suivant = deque->premier;
  deque->premier = e;
  if(deque->taille == 0){ //si la deque etait vide avant
    deque->dernier = e;
    e->suivant = e;
    e->prec = e;
  }
  deque->taille ++;
  */
  pthread_mutex_unlock (& deque->mutex);
}

//Defile la deque par en haut
Element* defilerHaut(struct Deque * deque){
  //pthread_mutex_lock (& deque->mutex);

  if (deque == NULL)
    exit(EXIT_FAILURE);

  Element * e = deque->premier; //l'element a retourner

  if(deque->taille == 1){
    deque->premier = NULL;
    deque->dernier = NULL;
  }else{
    //printf("%d\n",deque->taille);
    //printf("%p ",deque->premier);
    //printf("%p\n",deque->premier->suivant);
    deque->premier = deque->premier->suivant;
    deque->premier->prec = NULL;//
  }
  deque->taille --;

  /*
  if(deque->taille == 1){ //si c'est le seul dans la deque
    deque->dernier = NULL;
    deque->premier = NULL;
  }else{
    deque->premier->suivant = e;
    deque->premier = e->suivant;
    //deque->dernier->prec = deque->premier;
    deque->dernier->prec = NULL;
  }
  deque->taille --;
  */
  //pthread_mutex_unlock (& deque->mutex);

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
  e->suivant = NULL;

  if(deque->taille != 0){
    deque->dernier->suivant = e;
    e->prec = deque->dernier;
    e->suivant = NULL;
    deque->dernier = e;
  }else{
    e->prec = NULL;
    deque->premier = e;
    deque->dernier = e;
  }
  deque->taille++;
  /*
 e->prec = deque->dernier;
  e->suivant = NULL;
  //e->suivant = deque->premier;
  if(deque->taille == 0){ //si la deque etait vide avant
    deque->premier = e;
    e->suivant = e;
    e->prec = e;
  }else{
    deque->premier->prec = e;
    deque->dernier->suivant = e;
  }
  deque->dernier = e;
  deque->taille ++;
  */
  pthread_mutex_unlock (& deque->mutex);
}

//Defile la deque par en bas
Element* defilerBas(struct Deque * deque){
  pthread_mutex_lock (& deque->mutex);

  if (deque == NULL)
    exit(EXIT_FAILURE);

  Element * e = (Element*)malloc(sizeof(struct Element));

  if(deque->taille == 1){
    e = deque->premier;
    deque->premier = NULL;
    deque->dernier = NULL;
  }else{
    e = deque->dernier;
    deque->dernier = deque->dernier->prec;
    deque->dernier->suivant = NULL;//
  }
  deque->taille --;
  /*
  Element * e = deque->dernier; //l'element a retourner
 
  if(deque->taille == 1){ //si c'est le seul dans la deque
    deque->dernier = NULL;
    deque->premier = NULL;
  }else{
    deque->dernier = deque->dernier->prec;
    //deque->dernier->suivant = deque->premier;
  }
  deque->taille --;
  */
  pthread_mutex_unlock (& deque->mutex);

  return e;
}

void aux(void * arg){
  
  struct arg *args = (struct  arg *)arg;
  int id = idMythread(args->scheduler);

  while(1){
    int ws = 0;
    
    pthread_mutex_lock(& args->scheduler->mythreads[args->id].deque->mutex);
    int t = args->scheduler->mythreads[args->id].deque->taille;
    pthread_mutex_unlock(& args->scheduler->mythreads[args->id].deque->mutex);
    if(t == 0){ //deque vide
      /*
      pthread_mutex_lock(& args->scheduler->mutex);
      args->scheduler->nthreads_sleep ++;
      printf("AUX - %d\n",args->scheduler->nthreads_sleep);
      pthread_mutex_unlock(& args->scheduler->mutex);
      */
      args->scheduler->mythreads[id].sleep = 1;
      while(ws == 0){//tant que le work stealing echoue (ie return 0)
	ws = workStealing(args->scheduler,args->id);
	if(ws == 0)
	  usleep(1000); //dors 1ms avant de recommencer une etape de WS
      }	
    }else{ //sinon : la deque n'est pas vide

      //pthread_mutex_lock (& args->scheduler->mythreads[args->id].deque->mutex);
      Element * e = defilerBas(args->scheduler->mythreads[args->id].deque);
      //pthread_mutex_unlock (& args->scheduler->mythreads[args->id].deque->mutex);

      /*pthread_mutex_lock(& args->scheduler->mutex);
      args->scheduler->nthreads_sleep --;
      pthread_mutex_unlock(& args->scheduler->mutex);
      */
      e->t(e->closure, args->scheduler); //execute la tache depiler
    }
  }
}

int sched_init(int nthreads, int qlen, taskfunc f, void *closure){
  int i,t,pth;

  srand(time(0));

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
  scheduler->nthreads_sleep = 0;

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
    scheduler->mythreads[i].sleep = 0;
  }

  //Enfilement de la tache initiale dans la deque du premier thread
  //pthread_mutex_lock (& scheduler->mythreads[0].deque->mutex);
  enfilerHaut(scheduler->mythreads[0].deque,f,closure);
  //pthread_mutex_unlock (& scheduler->mythreads[0].deque->mutex);

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
  /*
  do{
    pthread_mutex_lock (& scheduler->mutex);
    t = scheduler->nthreads - scheduler->nthreads_sleep;
    pthread_mutex_unlock (& scheduler->mutex);
    usleep(1000);
  }while(t != 0); //ie tant qu'il y a des threads reveilles
*/
  while(allSleep(scheduler) == 0){
    usleep(10);
  }

  /*
  //Liberation de la memoire
  for(i = 0; i < scheduler->nthreads; i++)
    free(scheduler->mythreads[i].deque);
  free(scheduler->mythreads);
  free(scheduler);
*/

  return 1;
}

int sched_spawn(taskfunc f, void *closure, struct scheduler *s){
  int id = idMythread(s);

  if(id == -1)
    return -1; //erreur
  
  //enfilement de la tache dans la deque adequate
  //pthread_mutex_lock(& s->mythreads[id].deque->mutex);
  enfilerBas(s->mythreads[id].deque,f,closure);
  //pthread_mutex_unlock(& s->mythreads[id].deque->mutex);

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

  int r,i,r_i,t;
  Element * e;

  do{
    r = (rand()%s->nthreads);
  }while(r == id);

  pthread_mutex_lock(& s->mythreads[r].deque->mutex);
  t = s->mythreads[r].deque->taille;
  //pthread_mutex_unlock(& s->mythreads[r].deque->mutex);
  //printf("WS - R - %d\n",r);

  if(t != 0){
    
    /*
    pthread_mutex_lock(& s->mutex);
    s->nthreads_sleep --;
    printf("WS - %d\n",s->nthreads_sleep);
    pthread_mutex_unlock(& s->mutex);
    */
    s->mythreads[id].sleep = 0;
    
    e = defilerHaut(s->mythreads[r].deque);
    e->t(e->closure, s); //execute la tache

    //printf("Tache excutee\n");
    pthread_mutex_unlock(& s->mythreads[r].deque->mutex);

    //pthread_mutex_lock(& s->mutex);
    //s->nthreads_sleep ++;
    //pthread_mutex_unlock(& s->mutex);
    	
    return 1;
  }


  pthread_mutex_unlock(& s->mythreads[r].deque->mutex);

  for(i = 1; i < s->nthreads; i++){
    r_i = (r+i)%s->nthreads;
    pthread_mutex_lock(& s->mythreads[r_i].deque->mutex);
    int t = s->mythreads[r_i].deque->taille;
    pthread_mutex_unlock(& s->mythreads[r_i].deque->mutex);
    if(t != 0){

      s->mythreads[id].sleep = 0;

      e = defilerHaut(s->mythreads[r_i].deque);
      e->t(e->closure, s); //execute la tache 

      //pthread_mutex_unlock(& s->mythreads[r_i].deque->mutex);
      return 1;
    }
    //pthread_mutex_unlock(& s->mythreads[r_i].deque->mutex);
  }



  return 0; //echoue
}

int allSleep(struct scheduler * s){
  int i;
  for(i = 0; i < s->nthreads; i++)
    if(s->mythreads[i].sleep == 0)
      return 0;
  return 1;
}
