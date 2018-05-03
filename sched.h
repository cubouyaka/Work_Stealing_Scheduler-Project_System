#pragma once
#ifndef SCHED_H
#define SCHED_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>

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
  Element * prec;
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
