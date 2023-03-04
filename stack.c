#include "stack.h"
#include <stdlib.h>
#include <stdio.h>

//================================================
//
// Shawn Fahimi and David Ajanaku
//
// Project 4(a)(1) - Stack
// Computer System Fundamentals
//
//================================================

//================================================
// push a value onto the stack (char)
//================================================
void push(StackNodePtr *topPtr, char value){
    StackNodePtr temp = malloc(sizeof(StackNode));

    if(temp != NULL){
        temp->data = value;
        temp->nextPtr = *topPtr;
        *topPtr = temp;
    }
    else{
        printf("No memory available. \n");
    }
}

//================================================
// push a value onto the stack (int)
//================================================
void pushInt(StackNodePtr *topPtr, int value){
    StackNodePtr temp = malloc(sizeof(StackNode));

    if(temp != NULL){
        temp->sum = value;
        temp->nextPtr = *topPtr;
        *topPtr = temp;
    }
    else{
        printf("No memory available. \n");
    }
}

//================================================
// pop a value off the stack (char)
//================================================
char pop(StackNodePtr *topPtr){

    StackNodePtr temp = *topPtr;
    
    //David changed value to int
    //int value = (*topPtr)->data;
    char value = (*topPtr)->data;
    *topPtr = (*topPtr)->nextPtr;
    free(temp);
    return value;
}

//================================================
// pop a value off the stack (int)
//================================================
int popInt(StackNodePtr *topPtr){

    StackNodePtr temp = *topPtr;
    
    //David changed value to int
    int value = (*topPtr)->sum;
    //char value = (*topPtr)->data;
    *topPtr = (*topPtr)->nextPtr;
    free(temp);
    return value;
}

//================================================
// peek at the top value on the stack (char)
//================================================
char stackTop(StackNodePtr topPtr){
        //David changed value to int
        //int value = (topPtr)->data;
        char value = (topPtr)->data;
        return value;
        
}

//================================================
// returns 1 if the stack is empty, 0 otherwise
//================================================
int isEmpty(StackNodePtr topPtr){
    if(topPtr == NULL){
        return 1;
    }
    else{
        return 0;
    }
}

//================================================
// print the stack (chars)
//================================================
void printStack(StackNodePtr topPtr){
   
    if(topPtr != NULL){
        //StackNodePtr temp = topPtr;
        while(topPtr != NULL){
            //David changed %c to %d to display int instead of char
            printf("%c -> ", topPtr->data);
            //printf("%d -> ", topPtr->data);
            topPtr = topPtr->nextPtr;
        }
        printf("END\n");
    }
    else{
        printf("Empty stack.\n");
    }
}