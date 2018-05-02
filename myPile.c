#include "myPile.h"

void empiler(myPile *pile, void *tache){
  Element *nouveau = malloc(sizeof(*nouveau));
  if (pile == NULL || nouveau == NULL){
    exit(EXIT_FAILURE);
  }
  nouveau->tache = tache;
  nouveau->suivant = pile->premier;
  pile->premier = nouveau;
}

Element *depiler(myPile *pile){
    if (pile == NULL){
      exit(EXIT_FAILURE);
    }
    void *tache;
    Element *elementDepile = pile->premier;
    if (pile != NULL && pile->premier != NULL){
        tache = elementDepile->tache;
        pile->premier = elementDepile->suivant;
        free(elementDepile);
    }
    return tache;
}

void afficherPile(myPile *pile){
    if (pile == NULL){
        exit(EXIT_FAILURE);
    }
    Element *actuel = pile->premier;
    while (actuel != NULL){
        printf("%d\n",actuel->tache);// le nom de la tache
        actuel = actuel->suivant;
    }
    printf("\n");
}


int main()

{

    myPile *maPile;// = initialiser();
    empiler(maPile, 4);
    empiler(maPile, 8);
    empiler(maPile, 15);
    empiler(maPile, 16);
    empiler(maPile, 23);
    empiler(maPile, 42);
    printf("\nEtat de la pile :\n");
    afficherPile(maPile);
    printf("Je depile %d\n", depiler(maPile));
    printf("Je depile %d\n", depiler(maPile));
    printf("\nEtat de la pile :\n");
    afficherPile(maPile);
    return 0;

}
