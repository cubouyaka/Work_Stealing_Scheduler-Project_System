
#include <stdio.h>
#include <stdlib.h>

typedef struct Element Element;

struct Element{
	void *tache;
	Element *suivant;
};


typedef struct myPile myPile;

struct myPile{
	Element *premier;
};

void empiler(myPile *pile,void *tache);

Element *depiler(myPile *pile);


void affichePile(myPile *pile);
