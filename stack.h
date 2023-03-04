#ifndef stack
#define stack

struct stackNode {
    int sum; //added an int data field for use in postfix evaluation functions
    char data;
    struct stackNode *nextPtr;
};
typedef struct stackNode StackNode;
typedef StackNode *StackNodePtr;

//function descriptions in stack.c
void push(StackNodePtr *topPtr, char value);
void pushInt(StackNodePtr *topPtr, int value);
char pop(StackNodePtr *topPtr);
int popInt(StackNodePtr *topPtr);
char stackTop(StackNodePtr topPtr);
int isEmpty(StackNodePtr topPtr);
void printStack(StackNodePtr topPtr);

#endif