//================================================
//
// Shawn Fahimi and David Ajanaku
//
// Project 4(a)(2) - Infix to Postfix
// Computer System Fundamentals
//
//================================================

//================================================
// includes and defines
//================================================
#include "infix-to-postfix.h"
#include "stack.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

//================================================
// converts an infix expression stored in infix[]
// to a postfix expression, which is then stored
// in postfix[]
//================================================
void convertToPostfix(char infix[], char postfix[]){
    char el;
    StackNodePtr ns = NULL;
	int i = 0;
    int j = 0;
	push(&ns,'(');
	strcat(infix, ")");
	
	while(infix[i] != '\0'){
		if(infix[i] == '('){
			push(&ns,infix[i]);
		}
		else if(isdigit(infix[i])){
            while(infix[i] != '\0' && isdigit(infix[i])){
                postfix[j] = infix[i];
                j++;
                i++;
            }
            i--;
            postfix[j] = ' ';
            j++;
		}
		else if(isOperator(infix[i]) == 1){
            if(infix[i] != '-'){
			    el = pop(&ns);
			    while(isOperator(el) == 1 && precedence(el,infix[i]) >= 0){
				    postfix[j] = el;
				    j++;
                    postfix[j] = ' ';
                    j++;
				    el = pop(&ns);
			    }
			    push(&ns,el);
			    push(&ns,infix[i]);
            }
            else{
                if(!isEmpty(ns) && stackTop(ns) != '('){
                    el = pop(&ns);
			        while(isOperator(el) == 1 && precedence(el,infix[i]) >= 0){
				        postfix[j] = el;
				        j++;
                        postfix[j] = ' ';
                        j++;
				        el = pop(&ns);
			        }
			        push(&ns,el);
			        push(&ns,infix[i]);
                }
                else{
                    push(&ns, infix[i]);
                }
            }
		}
		else if(infix[i] == ')'){
			el = pop(&ns);
			while(el != '('){
				postfix[j] = el;
				j++;
                postfix[j] = ' ';
                j++;
				el = pop(&ns);
			}
		}
		else if(infix[i] == ' '){

		}
        else{
            printf("\nInvalid expression!");
			exit(1);
        }
		i++;
	}
	postfix[j] = '\0';
}

//================================================
// determines if c is an operator (1 if true)
//================================================
int isOperator(char c){
    if(c == '+' || c == '-' || c == '*' || c == '/' || c == '%'){
        return 1;
    }
    return 0;
}

//================================================
// determines the precedence order of operator1
// and operator2, returns 0 if operator1 is 
// equivalent in precedence to operator2, -1 if
// operator1 is lower in precedence than 
// operator2, and 1 if operator1 is greater in 
// precedence than operator2
//================================================
int precedence(char operator1, char operator2){
    if(operator1 == '+' || operator1 == '-'){
        if(operator2 == '+' || operator2 == '-'){
            return 0;
        }
        else if(operator2 == '*' || operator2 == '/' || operator2 =='%'){
            return -1;
        }
    }
    else if(operator1 == '*' || operator1 == '/' || operator2 == '%'){
        if(operator2 == '*' || operator2 == '/' || operator2 == '%'){
            return 0;
        }
        else if(operator2 == '+' || operator2 == '-'){
            return 1;
        }
    }
    else{
        return -99;
    }
    return -99;
}

//================================================
// returns the integer result of the postfix
// expression passed to the function in *expr
//================================================
int evaluatePostfixExpression(char *expr){
    StackNodePtr ns = NULL;
    int i = 0;
    while(expr[i] != '\0'){
        
        if(expr[i] == ' '){
            i++;
        }
        else if(isdigit(expr[i])){
            int num = 0;
            while(expr[i] != '\0' && isdigit(expr[i])){
                num = num * 10 + expr[i] - '0';
                i++;
            }
           
           /* The problem may be because push() takes char (8 bits maximum) instead of
            * an int (up to 32 bits), so push() is probably taking in num as a buffer 
            * overflow value due to char truncating the leading 24 bits.*/

            //David will
            pushInt(&ns, num);
        }
        else if(isalpha(expr[i])){

        }
        else if(isOperator(expr[i])){
            int result;
            int op1 = popInt(&ns);
            if(!isEmpty(ns)){
                int op2 = popInt(&ns);
                result = calculate(op2, op1, expr[i]);
            }
            else{
                result = calculate(0, op1, expr[i]);
            }
            pushInt(&ns, result);
            i++;
        }
    }
    return popInt(&ns);
}

//================================================
// computes and returns the result of 
// op1 operator op2
//================================================
int calculate(int op1, int op2, char operator){
    switch(operator){
        case '+':
            return op1 + op2;
        case '-': 
            return op1 - op2;
        case '*':
            return op1 * op2;
        case '/':
            return op1 / op2;
        case '%':
            return op1 % op2;
    }
    return -99;
}
