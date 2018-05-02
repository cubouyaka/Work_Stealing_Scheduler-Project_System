#include "lifo.h"

void empiler(struct Lifo * lifo, void * taskfunc){
  struct Element * e = (Element *) malloc(sizeof(struct Element));
  if (lifo == NULL || e == NULL)
    exit(EXIT_FAILURE);
  e->taskfunc = taskfunc;
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
