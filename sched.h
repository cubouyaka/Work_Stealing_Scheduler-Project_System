#pragma once 
#ifndef SCHED_H
#define SCHED_H

//#include "lifo.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

typedef void (*taskfunc)(void*, struct scheduler *);

typedef struct scheduler {
  int nthreads; //nombre de threads
  int qlen; //nombres minimum de taches simultanees
  pthread_t * threads ;//tableau de threads
  struct Lifo * lifo; //la pile d'execution (lifo)
} scheduler;

static inline int sched_default_threads(){
    return sysconf(_SC_NPROCESSORS_ONLN);
}

int sched_init(int nthreads, int qlen, taskfunc f, void *closure);
int sched_spawn(taskfunc f, void *closure, struct scheduler *s);


typedef struct Element {
  taskfunc t;
  void * closure;
  Element * prec;
} Element;

typedef struct Lifo {
  Element * dernier;
} Lifo;

void empiler(struct Lifo * lifo, taskfunc f, void *closure);

Element* depiler(struct Lifo * lifo);

int tailleLifo(struct Lifo * lifo);

#endif
