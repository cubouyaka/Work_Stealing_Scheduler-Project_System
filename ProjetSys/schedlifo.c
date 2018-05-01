#include <stdio.h>
#include <stdlib.h>
#include "sched.h"

int isFull(struct Stack* stack)
{   return stack->top == stack->capacity ; }

int isEmpty(struct Stack* stack)
{   return stack->top == -1;  }

//à verifier
int  push(struct Stack* stack,struct tskfunc* f)
{
//  struct tskfunc funct=(struct tskfunc)f;
    if(!isFull(stack)) {
      if(f->tsk!=NULL){
        stack->top =stack->top + 1;
        stack->array[stack->top] =f;
        if(stack->array[stack->top]->tsk==NULL){
          printf("pushed failed \n");
          return -1;
        }

        else
        {
          printf("pushed successfully \n");
          printf("top  = %d\n",stack->top);

          return 1;
        }
      }
      else
      {
        printf("the task is NULL \n");
        return -1;
      }


   } else {
      printf("Could not insert function, Stack is full.\n");
      return -1;
   }
}
struct tskfunc* pop(struct Stack* stack)
{
    if (isEmpty(stack)){
        printf("stack is empty\n");
        return NULL;
      }
    else
      {
        struct tskfunc* f=stack->array[stack->top--];
         printf("task  popped to stack\n");
         return f;
      }

}

struct Stack* createStack(unsigned capacity,struct tskfunc* f)
{

    struct Stack* stack = (struct Stack*) malloc(sizeof(struct Stack));
    stack->capacity = capacity;
    //a verifier
    stack->top = -1;
    //a regler :soit taskfunc ** ou taskfunc**
    //stack->array[capacity];

    stack->array[stack->capacity ] ;//=  malloc(stack->capacity* sizeof(struct tskfunc));
    printf (" Creation  !\n");

    // for (size_t i = 0; i < stack->capacity; i++) {
    //   printf (" des threads !\n");
    //   // stack->array[i]->cls=NULL;
    //   // stack->array[i]->tsk=NULL;
    //   stack->array[i]=NULL;
    //
    // }

    // for (size_t i = 0; i < stack->capacity; i++) {
    //   stack->array[i]->tsk=NULL;
    //
    //
    // }

     push(stack,f);
    // printf("top  = %d\n",stack->top);

    return stack;
}

void print(struct Stack* stack)
{
  int i;
  for ( i = 0; i <= stack->top; i++) {
    if(stack->array[i]->tsk==NULL)
      printf("NULL \n");
    else
      printf(" tache N° %d\n",i);

  }
}

//creer une focntion qui ns retourne le nombrre de tache dans la fonxtion :facultatif car top ns le donne

/*int main()
{
   taskfunc task1=5;
   taskfunc task2=5;
   taskfunc task3=5;
   taskfunc task4=5;


   struct Stack* stack = createStack(5,task1);
   print(stack);
   push(stack, task3);
    print(stack);
    //printf("top=  %d\n",stack->top);
     push(stack, task2);
      print(stack);
      // printf("top=  %d\n",stack->top);
       push(stack, task4);
        print(stack);
         printf("top=  %d\n",stack->top);
    push(stack, task4);

  //  push(stack, 30);
  print(stack);
    pop(stack);
   print(stack);
    pop(stack);
    print(stack);
    pop(stack);
    print(stack);
    pop(stack);

    return 0;
}*/
