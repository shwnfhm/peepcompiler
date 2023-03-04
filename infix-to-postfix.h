#include "stack.h"

//function descriptions in infix-to-postfix.c
void convertToPostfix(char infix[], char postfix[]);
int isOperator(char c);
int precedence(char operator1, char operator2);

int evaluatePostfixExpression(char *expr);
int calculate(int op1, int op2, char operator);