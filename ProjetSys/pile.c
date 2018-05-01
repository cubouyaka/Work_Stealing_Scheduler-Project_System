
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

//Structure de pile
struct Stack
{
    int top;
    unsigned capacity;
    int* array;
};

// function to create a stack of given capacity. It initializes size of
// stack as 0
struct Stack* createStack(unsigned capacity)
{
    struct Stack* stack = (struct Stack*) malloc(sizeof(struct Stack));
    stack->capacity = capacity;
    stack->top = -1;
    stack->array = (int*) malloc(stack->capacity * sizeof(int));
    return stack;
}

// Stack is full when top is equal to the last index
int isFull(struct Stack* stack)
{   return stack->top == stack->capacity - 1; }

// Stack is empty when top is equal to -1
int isEmpty(struct Stack* stack)
{   return stack->top == -1;  }

// Function to add an item to stack.  It increases top by 1
void push(struct Stack* stack, int item)
{
    if (isFull(stack))
        return;
    stack->array[++stack->top] = item;
    printf("%d pushed to stack\n", item);
}
void print(struct Stack* stack)
{   int i;
  for ( i = 0; i <= stack->top; i++) {
    printf(" element n %d\n", stack->array[i]);

  }
}

// Function to remove an item from stack.  It decreases top by 1
void pop(struct Stack* stack)
{
    if (isEmpty(stack)){
        printf("stack is empty\n");
        //return;
      }
    else
    {
      int val= stack->array[stack->top--];
       printf("%d popped to stack\n",val);
    }


}

// Driver program to test above functions
int main()
{
    struct Stack* stack = createStack(4);

    push(stack, 10);
    push(stack, 20);
    push(stack, 30);
    print(stack);
    pop(stack);
    print(stack);
    pop(stack);
    print(stack);
    pop(stack);
    pop(stack);
    pop(stack);
    pop(stack);
    pop(stack);
    pop(stack);
    return 0;
}
