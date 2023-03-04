//================================================
//
// Shawn Fahimi and David Ajanaku
//
// Project 4(a)(3) - Infix to Postfix Tester
// Computer System Fundamentals
//
//================================================
#include "infix-to-postfix.h"
#include "stack.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//================================================
// This module tests the infix-to-postfix.h/.c 
// module implemented according to the 
// specifications in the infix-to-postfix.md file.
// It can handle multi-digit integers but not
// variable names (variable handling is implemented
// in peepcc.c). Upon running ./test-i2p, the 
// function will prompt you to enter a valid infix
// expression, and if such an expression is entered,
// the program will return the correct postfix
// form of the expression along with its 
// evaluation result using the functions in 
// infix-to-postfix.c
//================================================
int main(int argc, char** argv){
    char infix[100];
    char postfix[100];
	printf("Enter a valid infix expression (no variables): ");
	fgets(infix,100,stdin);
	int n = strlen(infix);

	if (n > 0 && infix[n - 1] == '\n') {
  		infix[n - 1] = '\0';
	} 	
	
	printf("\nInfix Expression is: %s", infix);
	convertToPostfix(infix, postfix);
	printf("\nEquivalent postfix expression is: %s", postfix);
	printf("\nEvaluation: %d\n", evaluatePostfixExpression(postfix));
    return(0);
}