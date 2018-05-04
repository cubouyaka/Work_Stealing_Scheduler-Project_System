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
  // les mutexs
  pthread_mutex_t mutex;
  pthread_mutex_t mutex_sleep;
  pthread_cond_t cond; // condition d'arret pour les mutex
  int nbre_th_sleep; //nombre de thread qui dorment
} scheduler;

typedef void (*taskfunc)(void*, struct scheduler *);
static inline int sched_default_threads(){
    return sysconf(_SC_NPROCESSORS_ONLN);
}

int sched_init(int nthreads, int qlen, taskfunc f, void *closure);
int sched_spawn(taskfunc f, void *closure, struct scheduler *s);

typedef struct Element {
  taskfunc t;
  void * closure;
  struct Element * prec;
} Element;

typedef struct Lifo {
  int taille; //nombre d'elements dans la pile
  Element * dernier;
  pthread_mutex_t mutex;
} Lifo;

//Empile la taskfunc f dans la pile d'execution
void empiler(struct Lifo * lifo, taskfunc f, void *closure);
//Depile un element de la pile d'execution
Element* depiler(struct Lifo * lifo);

#endif
