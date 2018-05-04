#pragma once
#ifndef SCHED_H
#define SCHED_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>



typedef struct scheduler {
  int nthreads; //nombre de threads
  int qlen; //nombres minimum de taches simultanees
  pthread_t * threads ;//tableau de threads
  struct Lifo * lifo; //la pile d'execution (lifo)
  // les mutex
  pthread_mutex_t mutex;
  pthread_mutex_t mutex_depiler;
  pthread_mutex_t mutex_empiler;
  pthread_mutex_t mutex_sleep;
  pthread_mutex_t mutex_unsleep;
  // conditions d'arrets pour les mutex
  pthread_cond_t cond;
  // compteur des threads qui dorment
  int nbre_th_sleep;
} scheduler;
typedef void (*taskfunc)(void*, struct scheduler *);
static inline int sched_default_threads(){
    return sysconf(_SC_NPROCESSORS_ONLN);
}

/*
struct arguments {
  taskfunc t;
  void * closure;
  struct scheduler * scheduler;
};
*/

int sched_init(int nthreads, int qlen, taskfunc f, void *closure);
int sched_spawn(taskfunc f, void *closure, struct scheduler *s);

typedef struct Element {
  taskfunc t;
  void * closure;
  struct Element * prec;
} Element;

typedef struct Lifo {
  Element * dernier;
} Lifo;

//Empile la taskfunc f dans la pile d'execution
void empiler(struct Lifo * lifo, taskfunc f, void *closure);
//Depile un element de la pile d'execution
Element* depiler(struct Lifo * lifo);
//Renvoi le nombre d'elements dans la pile d'execution
int tailleLifo(struct Lifo * lifo);

#endif
