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
void  aux(void * s ){
  struct scheduler *scheduler=(struct  scheduler *)s;;

  while(1)
  {
    pthread_mutex_lock (& scheduler->mutex_depiler);
    int compteur = tailleLifo(scheduler->lifo);
     pthread_mutex_unlock(& scheduler->mutex_depiler);

    if((compteur)==0)
    {
      //pile vide
      pthread_mutex_lock (& scheduler->mutex_sleep);
      //incrementer les threads qui dorment
      scheduler->nbre_th_sleep++;
      pthread_mutex_unlock(& scheduler->mutex_sleep);
      //sleep
      pthread_cond_wait (& scheduler->cond, & scheduler->mutex);

      pthread_mutex_lock (& scheduler->mutex_unsleep);
       //decrementer les threads qui dorment
      scheduler->nbre_th_sleep--;
      pthread_mutex_unlock (& scheduler->mutex_unsleep);
    }
    else{
      pthread_mutex_lock (& scheduler->mutex_depiler);
      struct Element * e=depiler(scheduler->lifo);
      pthread_mutex_unlock (& scheduler->mutex_depiler);
      //executer la taches depiler
      e->t(e->closure, scheduler);

    }

  }
  //return NULL;
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


  //initialisation les threads qui dorment
  scheduler->nbre_th_sleep=0;

  //Empilement de la tache initiale sur la pile
  empiler(scheduler->lifo,f,closure);
  //initialisation du mutex
  scheduler->mutex=(pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
  scheduler->mutex_depiler=(pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
  scheduler->mutex_empiler=(pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
  scheduler->mutex_sleep=(pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
  scheduler->mutex_unsleep=(pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

  //la condition d'attente des threeads
  scheduler->cond= (pthread_cond_t)PTHREAD_COND_INITIALIZER;

  //Execution de la pile tant qu'il y a des taches a faire
  //Termine lorsque la pile est vide ET tous les threads sont endormis
  //struct Element * e = (struct Element *) malloc(sizeof(struct Element));
  //int nb_threads_actuel = 0;
  /*while(scheduler->lifo->dernier != NULL){
    e = depiler(scheduler->lifo);
    //e->t(e->closure,scheduler);
    if(nb_threads_actuel < nthreads){

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

    }
  }*/
  int i;
  int pth;
  fprintf(stderr,"coeur %d \n",nthreads);
  for (i = 0; i < scheduler->nthreads; i++)
        {
            fprintf(stderr,"ite %d \n",i);
             pth = pthread_create (&scheduler->threads[i], NULL,(void* )aux,(void * )scheduler);
             if (pth)
             {
                //fprintf (stderr, "%s", strerror (pth));
             }
        }

        pthread_mutex_lock (& scheduler->mutex);
        fprintf(stderr,"sleep  init %d \n",scheduler->nbre_th_sleep);
        fprintf(stderr,"taille init %d \n",tailleLifo(scheduler->lifo));
        //retourner lorsque tous les thread sont endormi et la pile vide.
        while((tailleLifo(scheduler->lifo)!=0) || (scheduler->nbre_th_sleep== nthreads)){
            fprintf(stderr,"taille %d \n",tailleLifo(scheduler->lifo));
            fprintf(stderr,"sleep  init %d \n",scheduler->nbre_th_sleep);
        }
        pthread_mutex_unlock (& scheduler->mutex);
          free(scheduler->lifo->dernier);
          free(scheduler->lifo);
          free(scheduler->threads);
          free(scheduler);
          return 1;
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
  }else{
    //lock à faire
    pthread_mutex_lock(& s->mutex_depiler);
    // empiler la nouvelle tache
    empiler(s->lifo,f,closure);

    //signal à faire pour reveiller les threads qui dorment
     pthread_cond_signal (& s->cond);
    //unlock à faire
    pthread_mutex_unlock(& s->mutex_depiler);
    return 0;
  }
}
