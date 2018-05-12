#pragma once
#ifndef SCHED_H
#define SCHED_H

#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

typedef struct MyThread {
  pthread_t thread;
  struct Deque * deque;
  int id;
  int sleep; //0 if false 1 else
} MyThread;

typedef struct scheduler {
  int nthreads; //nombre de threads
  int qlen; //nombres minimum de taches simultanees
  MyThread * mythreads;
  pthread_mutex_t mutex;
  int nthreads_sleep; //le nombre de threads endormis
} scheduler;

typedef struct arg{
  struct scheduler* scheduler;
  int id;
} arg;

typedef void (*taskfunc)(void*, struct scheduler *);

typedef struct Element {
  taskfunc t;
  void * closure;
  struct Element * prec;
  struct Element * suivant;
} Element;

typedef struct Deque {
  int taille; //nombre d'elements dans la deque
  Element * premier;
  Element * dernier;
  pthread_mutex_t mutex;
} Deque;


static inline int
sched_default_threads(){ return sysconf(_SC_NPROCESSORS_ONLN); }

int sched_init(int nthreads, int qlen, taskfunc f, void *closure);
int sched_spawn(taskfunc f, void *closure, struct scheduler *s);

//Etape de Work Stealing : retourne 0 si echoue, 1 sinon
int workStealing(struct scheduler * s, int id);

//Retourne l'id du thread courant dans le tableau mythreads
int idMythread(struct scheduler * s);

//1 si tout les threads sont endormis, 0 sinon
int allSleep(struct scheduler * s);

//Enfile la taskfunc f en haut de la deque 
void enfilerHaut(struct Deque * deque, taskfunc f, void *closure);
//Defile la deque par en haut
Element* defilerHaut(struct Deque * deque);

//Enfile la taskfunc f en bas de la deque 
void enfilerBas(struct Deque * deque, taskfunc f, void *closure);
//Defile la deque par en bas
Element* defilerBas(struct Deque * deque);


#endif
