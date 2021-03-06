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
  int sleep; //1 si il dors, 0 sinon
  int nb_t_eff; //nombre de taches effectuees
  int ws_success; //nombre d'etapes de work stealing reussies
  int ws_fail; //nombre d'etapes de work stealing echouees
} MyThread;

typedef struct scheduler {
  int nthreads; //nombre de threads
  int qlen; //nombres minimum de taches simultanees
  MyThread * mythreads; //les threads
  pthread_mutex_t mutex; //primitive de synchronisation
  int nthreads_sleep; //le nombre de threads endormis
} scheduler;

typedef struct arg{
  struct scheduler* scheduler;
  int id;
} arg;

typedef void (*taskfunc)(void*, struct scheduler *);

typedef struct Element {
  taskfunc t; //la fonction a execute
  void * closure; //les arguments de la fonction
  struct Element * prec; //l'element precedent dans la pile
  struct Element * suivant; //l'element suivant dans la pile
} Element;

typedef struct Deque {
  int taille; //nombre d'elements dans la deque
  Element * premier;
  Element * dernier;
  pthread_mutex_t mutex; //primitive de synchronisation
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

//Affiche les stats par threads
void stats(struct scheduler * s);


#endif
