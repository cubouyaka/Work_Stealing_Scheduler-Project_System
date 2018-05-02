#include <stdio.h>
#include <stdlib.h>

typedef struct Element {
  void * taskfunc;
  Element * prec;
} Element;

typedef struct Lifo {
  Element * dernier;
} Lifo;

void empiler(struct Lifo * lifo, void * taskfunc);

Element* depiler(struct Lifo * lifo);

int tailleLifo(struct Lifo * lifo);
