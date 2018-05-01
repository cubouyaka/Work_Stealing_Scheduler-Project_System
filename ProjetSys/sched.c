#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "schedlifo.c"

//void * cls;

void fonct(void *inf ){
  struct info *arguments= (struct info *)inf;
  struct scheduler *sched=arguments->s;
  // struct tskfunc *task=arguments->func;
  // taskfunc func=task->tsk;
  //sched=arguments->s;
  //void *closure = (void *)arguments->closure_;
  struct tskfunc* f;
  pthread_mutex_lock (& sched->mutex_);

  // if(cls==NULL)
  //   printf("closure null\n" );
  // //while(1){

    if(isEmpty(sched->stk))
    {
      printf("  pile vide\n");
    //  sleep(5);
   pthread_cond_wait (& sched->cond_, & sched->mutex_);
    }

      f=pop(sched->stk);

      if(f->tsk!=NULL)
        f->tsk(f->cls, sched);
      if(f->tsk==NULL)
        printf("f est NULL\n");



//  }

return;
}
int sched_init(int nthreads, int qlen, taskfunc f, void *closure)
  {

    int i;
    int pth;
    if(nthreads<=0)
      {
        nthreads=sched_default_threads();
      }
      struct scheduler *sched= (struct scheduler*) malloc(sizeof(struct scheduler));
      if(sched==NULL)
        printf("error threads\n");
      sched->nb_thread =nthreads;
      sched->q_len = qlen;

      struct tskfunc* taskfunc=(struct tskfunc*) malloc(sizeof(struct tskfunc));
      taskfunc->cls=closure;
      taskfunc->tsk=f;
      //creer une pile avec une tache initiale

      //initialisation du mutex
      sched->mutex_=(pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
      //la condition d'attente des threeads
      sched->cond_= (pthread_cond_t)PTHREAD_COND_INITIALIZER;

      //creer les threads avant de lancer la tache f
      printf ("Creation des threads !\n");
      sched->thread = malloc(sched->nb_thread * sizeof(pthread_t));

      sched->stk =createStack(sched->q_len ,taskfunc  ) ;
      printf (" Creation  !\n");

      //la tache qu'on vas pop dans la pile,
      //taskfunc fonct;
      //les paramettres de la tache qui sera passé en arguments a un thread
      struct info* inf=(struct info*) malloc(sizeof(struct info));
      if(inf==NULL)
        printf("error info\n");
      ///inf->closure_=closure;
      inf->s=sched;
      //cls=closure;
      printf ("commencement\n");
      printf ("nb_thread= %d\n",sched->nb_thread);
      printf ("\n");

     for (i = 0; i < sched->nb_thread; i++)
      {  printf("iteration N° %d\n",i);
            //ne pas oublier de changer le prototype de la tache quicksort
           pth = pthread_create (& sched->thread [i], NULL,(void* )fonct,(void * )inf);

           if (pth)
           {
              fprintf (stderr, "%s", strerror (pth));
           }


      }

      //faire attendre les threads
      for (i = 0; i < sched->nb_thread; i++)
      {
        printf("attente du thread %d\n",i );
        pthread_join ( sched->thread[i], NULL);
      }
      free(sched->stk);
      free(taskfunc);
      free(inf);
      free(sched->thread);
      free(sched);

      if(isEmpty(sched->stk))      //retourner lorsque tous les thread sont endormi et la pile vide.
        {

            return 1;
        }

      return 0;
  }
int sched_spawn(taskfunc f, void *closure, struct scheduler *s)
    {
      int ret;

      //verifié si la taille de la pile est >= a la taile du scheduler
      if(s->stk->top >= s->q_len){
        printf("plein\n" );
        return -1;
      }

      //inseré la tache f(closure,s) dans la pile s->stk
      else
      {     // pthread_mutex_unlock(& s->mutex_);
        struct tskfunc* task=(struct tskfunc*) malloc(sizeof(struct tskfunc));
        task->tsk=f;
        task->cls=closure;
        printf("tache ajouté dans la pile\n");
        //trouver un moyen d'envoyer inf
        ret= push(s->stk,task);
        pthread_mutex_unlock(& s->mutex_);

         pthread_cond_signal (& s->cond_);
        free(task);
        return ret;

      }


    }
