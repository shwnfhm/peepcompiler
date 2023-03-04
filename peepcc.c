//================================================
//
// Shawn Fahimi and David Ajanaku
//
// Project 4(b) - Peep Compiler
// Computer System Fundamentals
//
//================================================

//================================================
// includes and defines
//================================================
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "infix-to-postfix.h"
#include "stack.h"

//HML op-codes
const unsigned int ADD = 0x10;
const unsigned int SUB = 0x11;
const unsigned int MUL = 0x12;
const unsigned int DIV = 0x13;
const unsigned int MOD = 0x14;
const unsigned int LOAD = 0x40;
const unsigned int STORE = 0x41;
const unsigned int READ = 0x50;
const unsigned int WRITE = 0x51;
const unsigned int B = 0x30;
const unsigned int BNEG = 0x31;
const unsigned int BPOS = 0x32;
const unsigned int BZRO = 0x33;
const unsigned int HALT = 0xFF;

//PEEP instruction/symbol types
const unsigned int REM = 0;
const unsigned int CONST = 1;
const unsigned int VAR = 2;
const unsigned int LINE = 3;
const unsigned int INPUT = 4;
const unsigned int PRINT = 5;
const unsigned int LET = 6;
const unsigned int IF = 7;
const unsigned int LESSTHAN = 8;
const unsigned int LESSEQUAL = 9;
const unsigned int EQUAL = 10;
const unsigned int GREATEREQUAL = 11;
const unsigned int GREATERTHAN = 12;
const unsigned int GOTO = 13;
const unsigned int END = 14;
const unsigned int ERR = 15;
const unsigned int NOTEQUAL = 16;

//struct used to hold symbols
struct tableEntry {
    int symbol;
    char type; /* 'C', 'L', or 'V' */
    int location; /* 00 to FF */
    bool isEmpty; // tells us if the tableEntry is empty
};
typedef struct tableEntry TableEntry;

//================================================
// function prototypes
//================================================

//================================================
// compiles the loaded Peep program and saves
// the HML code generated during compilation to 
// file "output.hml"
//================================================
void compile(FILE *peepFile);

//================================================
// initializes compilerMemory (an array holding the
// statements in the Peep file), HMLMemory (an array
// holding the compiler output HML instructions), 
// SymbolTable (a TableEntry array serving as the
// symbol table for the compiler), and flags (array 
// that holds values corresponding to HMLMemory indicating
// which lines in HMLMemory after the first pass need 
// to be resolved in the second pass)
//================================================
void initialize(char *compilerMemory[], unsigned int HMLMemory[], TableEntry SymbolTable[], int flags[]); 

//================================================
// loads all Peep statements in peepFile 
// into compilerMemory
//================================================
void loadProgram(FILE *peepFile, char *compilerMemory[]);

//================================================
// performs the compiler first pass, which includes
// populating the symbol table represented by 
// SymbolTable and creating HML instructions 
// for the Peep statements in compilerMemory 
// and adding them to HMLMemory, while modifying 
// values in flags to reflect HML instructions that
// need to be resolved in the second pass
//================================================
void firstPass(unsigned int HMLMemory[], char *compilerMemory[],
                TableEntry SymbolTable[], int flags[],
                unsigned int *instructionCounterPtr);

//================================================
// performs the compiler's second pass by resolving
// any unresolved references generated in the first
// pass, and then outputting the final compiler output
// stored in HMLMemory to file "output.hml"
//================================================
void secondPass(unsigned int HMLMemory[], TableEntry SymbolTable[], int flags[]);

//================================================
// helper function that prints out the symbol table
// for the compiler
//================================================
void printSymbolTable(TableEntry SymbolTable[]);

//================================================
// helper function for the first pass that parses 
// the tokens in peepStatement (delimited by ' ') 
// into tokenizedString
//================================================
char **tokenize(char **tokenizedString, char *peepStatement);

//================================================
// helper function for the first pass that uses the
// previously defined Peep statement constants to
// indicate the token's (of a Peep statement) "type" 
//================================================
unsigned int getTokenType(char *token, unsigned int tokenPosition,
                            TableEntry SymbolTable[],
                            int **memoryLocationPtr,
                            unsigned int *repeatValuePtr, bool goTo);

//================================================
// searches the SymbolTable for the symbol indicated
// by char *token. returns 0 if not found
//================================================
int searchTable(TableEntry SymbolTable[], unsigned int symCode, char *token, int **memoryLocationPtr);

//================================================
// adds the symbol with the value indicated by 
// char *token into SymbolTable, with *symbolCounter
// and *lineCounterPtr used to determine the 
// location of each symbol in HML Memory
//================================================
unsigned int addSymbol(unsigned int tokenValue, char *token,
                        TableEntry SymbolTable[], unsigned int *symbolCounter,
                        unsigned int *lineCounterPtr, bool goTo);

//================================================
// helper function for the second pass that iterates
// through flags array for unresolved references in 
// HMLMemory and resolves them accordingly using
// values in SymbolTable
//================================================
void resolveReferences(int flags[], unsigned int HMLMemory[],
                        TableEntry SymbolTable[]);

//================================================
// converts an infix expression stored in infix[]
// to a postfix expression that is stored in 
// postfix[] after execution
//================================================
void convertToPostfixPeep(char infix[], char postfix[]);

//================================================
// evaluates a postfix expression by updating 
// HMLMemory with the appropriate instructions
//================================================
int evaluatePostfixExpressionPeep(unsigned int HMLMemory[], TableEntry SymbolTable[],
                                    char postfix[], unsigned int *instructionCounterPtr, int flags[], int loc);

//================================================
// helper function for the second pass that outputs
// the final compiler output in HMLMemory to file 
// "output.hml"
//================================================
void saveFile(unsigned int HMLMemory[]);

//================================================
// helper function for postfix evaluation that
// loads HML instructions corresponding to 
// arithmetic expressions into HMLMemory
//================================================
int loadInstruction(unsigned int HMLMemory[], unsigned int operand1, unsigned int operand2,
                        char operator1, unsigned int *instructionCounterPtr, int flags[], int loc);

//================================================
// helper function for loadInstruction that
// determines the next empty position in
// HMLMemory that can be used to allocate space
// for intermediate values in arithmetic 
// expressions
//================================================
int nextEmpty(unsigned int HMLMemory[], int flags[]);

//================================================
// helper function for dealing with parentheses
// in expressions without it afffecting the 
// symbol table operations
//================================================
void removeChar(char* s, char c);

void compile(FILE *peepFile){
    
    char *compilerMemory[256]; //holds Peep statements
    TableEntry symbolTable[256]; //holds symbols
    int flags[256]; //holds flags to unresolved references for second pass
    unsigned int HMLMemory[256]; //holds compiled HML instructions
    unsigned int instructionCounter = 0; //counter of HML instructions in memory
    unsigned int *instructionCounterPtr = &instructionCounter;

    //initialize the symbol table, compiler memory, HML memory and flags array
    initialize(compilerMemory, HMLMemory, symbolTable, flags);

    //load the Peep program into the compiler memory
    loadProgram(peepFile, compilerMemory);

    //do the compiler's first pass
    firstPass(HMLMemory, compilerMemory, symbolTable, flags, instructionCounterPtr);

    //do the compiler's second pass
    secondPass(HMLMemory, symbolTable, flags);
}

void initialize(char *compilerMemory[], unsigned int HMLMemory[], TableEntry SymbolTable[], int flags[]){
    
    //loop through compilerMemory, HMLMemory, SymbolTable, and flags
    //and set initial values for each
    for(int i = 0; i < 256; i++){
        SymbolTable[i].symbol = 00;
        SymbolTable[i].type = '\0';
        SymbolTable[i].location = 00;
        SymbolTable[i].isEmpty = true;

        compilerMemory[i] = calloc(1, sizeof(char));
        strcpy(compilerMemory[i], "()");

        HMLMemory[i] = 0;
        flags[i] = -1;
    }

}

void loadProgram(FILE *peepFile, char *compilerMemory[]){
    
    int i = 0, j = 0;
        
        //iterate through the lines of the Peep file 
        //and add each line to compilerMemory, character
        //by character
        while(!feof(peepFile)){
            fscanf(peepFile, "%c", &compilerMemory[i][j]);

            if(compilerMemory[i][j] == '\n'){
                compilerMemory[i][j] = '\0';
                i++;
                j = 0;
            }
            else{
                j++;
            }
            compilerMemory[i] = realloc(compilerMemory[i], (j+1) * sizeof(char));
        }
}

void firstPass(unsigned int HMLMemory[], char *compilerMemory[],
                TableEntry SymbolTable[], int flags[],
                unsigned int *instructionCounterPtr){
    
    char **tokenizedString; //Peep statement to be tokenized
    char **tempString; //temporary address of tokenized string
    char *infix; //holds infix expression
    char *postfix; //holds postfix expression
    unsigned int tokenNumber; //number of tokens in the tokenized string
    unsigned int tokenValue; //holds return value for getTokenType
    unsigned int tableIndex = 0; //position of element in symbol table
    unsigned int tokenPosition; //location of token in tokenized string
    int memoryLocation = 0; 
    unsigned int repeatValue = 0; //indicates that a token is a repeat
    unsigned int symbolCounter = 0; //tracks number of constants/variables
    bool let; //whether current Peep statement is an assignment
    bool branch; //whether current Peep statement is an "if"
    bool goTo; //whether current Peep statement is a goto or cond
    bool lessEqual; //indicates whether the token is less than or equal
    bool greaterEqual; //indicates whether the token is greater than or equal
    bool notEqual; //indicates whether token is not equal
    int op1; //first operator
    int op2; //second operator
    unsigned int lineCounter = 0;
    unsigned int *tableIndexPtr = &tableIndex; //pointer to table index
    unsigned int *repeatValuePtr = &repeatValue; //pointer to repeat value
    unsigned int *symbolCounterPtr = &symbolCounter; //pointer to symbol counter
    unsigned int *lineCounterPtr = &lineCounter; //pointer to line counter
    int *memoryLocationPtr = &memoryLocation; //pointer to memory location
    unsigned int count1;
    bool flag1;
    int loc;
   

    //iterate through compiler memory and tokenize each Peep statement
    for(int i = 0; i < 256; i++){
        infix = calloc(1, sizeof(char));
        postfix = calloc(1, sizeof(char));
        tokenNumber = 0;
        tokenPosition = 0;
        count1 = 0;
        char currLine[20];

        //reset all flags
        flag1 = false, let = false, branch = false, goTo = false, lessEqual = false;
        notEqual = false;
        greaterEqual = false;
        op1 = -1, op2 = -1;
        loc = 0;

        //checks if end of compilerMemory has been reached
        if(!strcmp(compilerMemory[i], "()")){
            break;
        }

        //tokenize the current Peep statement
        tokenizedString = tokenize(tokenizedString, compilerMemory[i]);
        tempString = tokenizedString;

        //count number of tokens
        while(*tempString != NULL){
            tokenNumber++;
            tempString++;
        }
        strcpy(currLine,*tokenizedString);
        //iterate through each token in the Peep statement to determine 
        //how to modify the symbol table and/or update HML memory
        while(*tokenizedString != NULL){
            
            
            //get the type of the current token 
            tokenValue = getTokenType(*tokenizedString, tokenPosition, SymbolTable, 
                                        &memoryLocationPtr, repeatValuePtr, goTo);
          
            //ignore rest of Peep statement if it's a remark
            if(tokenValue == REM){
                break;
            }

            //if the current Peep statement is an END
            if(tokenValue == END){
                HMLMemory[*instructionCounterPtr] = 0x100 * HALT;
            }

            if(tokenValue == ERR && !isOperator(*tokenizedString[0]) && *tokenizedString[0] != '='){
                printf("SYNTAX ERROR AT LINE %s, TOKEN: %s\n", currLine, *tokenizedString);
                printf("Compilation unsuccessful\n");
                exit(0);
            }

            //if the current token is an already tabulated
            //constant or variable
            if(tokenValue == -CONST || tokenValue == -VAR){
                *tableIndexPtr = *repeatValuePtr;
                if(let == true && tokenValue == -VAR && tokenPosition == 2){
                    loc = SymbolTable[*tableIndexPtr].location;
                }
            }

            //if the token is a variable not already in the symbol table
            if(tokenValue == VAR){
                char tok[20];
                strcpy(tok, *tokenizedString);
                removeChar(tok, '(');
                removeChar(tok, ')');
                *tableIndexPtr = addSymbol(tokenValue, tok,
                                                SymbolTable, symbolCounterPtr, lineCounterPtr, goTo);
                
                if(flag1 == true){
                    count1++;
                }
            }

            //if the token is a constant not already in the symbol table
            if(tokenValue == CONST){
                if(strcmp(*tokenizedString, "+") && strcmp(*tokenizedString, "=") && strcmp(*tokenizedString, "*") && strcmp(*tokenizedString, "-") 
                                && strcmp(*tokenizedString, "/") && strcmp(*tokenizedString, "\%")){
                    char tok[20];
                    strcpy(tok, *tokenizedString);
                    removeChar(tok, '(');
                    removeChar(tok, ')');
                    *tableIndexPtr = addSymbol(tokenValue, tok, SymbolTable,
                                                symbolCounterPtr, lineCounterPtr, goTo);
                    if(flag1 == true){
                        count1++;
                    }
                }
               
            }

            //if the token is a line number not already in the symbol table
            if(tokenValue == LINE){
                *tableIndexPtr = addSymbol(tokenValue, *tokenizedString, SymbolTable,
                                                symbolCounterPtr, instructionCounterPtr, goTo);
            }

            if(tokenValue == CONST){
                if(strcmp(*tokenizedString, "+") && strcmp(*tokenizedString, "*") && strcmp(*tokenizedString, "-") 
                                && strcmp(*tokenizedString, "/") && strcmp(*tokenizedString, "%")){
                    if(SymbolTable[*tableIndexPtr].symbol < 0){
                        HMLMemory[256 - *symbolCounterPtr - 1] = SymbolTable[*tableIndexPtr].symbol - 0xFFFF0000;
                        (*symbolCounterPtr)++;
                    }
                    else{
                        HMLMemory[256 - *symbolCounterPtr - 1] = SymbolTable[*tableIndexPtr].symbol;
                        (*symbolCounterPtr)++;
                    }
                }
            }

            if(tokenValue == VAR){
                HMLMemory[256 - *symbolCounterPtr - 1] = SymbolTable[*tableIndexPtr].symbol;
                (*symbolCounterPtr)++;
                if(let == true && tokenPosition == 2){
                    loc = SymbolTable[*tableIndexPtr].location;
                }
            }


            if((tokenValue == -VAR || tokenValue == VAR) 
                    && tokenPosition == 2 && HMLMemory[*instructionCounterPtr] == 0x100 * READ){
                        HMLMemory[*instructionCounterPtr] += SymbolTable[*tableIndexPtr].location;
                        (*instructionCounterPtr)++;
            }

            //if the Peep statement is an input statement
            if(tokenValue == INPUT){
                HMLMemory[*instructionCounterPtr] = 0x100 * READ;
            }

            //if the Peep statement is a print statement
            if(tokenValue == PRINT){
                HMLMemory[*instructionCounterPtr] = 0x100 * WRITE;
            }

            if((tokenValue == -VAR || tokenValue == VAR) && tokenPosition == 2
                    && HMLMemory[*instructionCounterPtr] == 0x100 * WRITE){
                        HMLMemory[*instructionCounterPtr] += SymbolTable[*tableIndexPtr].location;
                        (*instructionCounterPtr)++;
            }
            
            //if the Peep statement is a let statement
            if(tokenValue == LET){
                let = true;
                flag1 = true;
            }

            if(let == true){
        
                if(tokenPosition >= 4){
                    infix = realloc(infix, (strlen(infix) + strlen(*tokenizedString) + 2) * sizeof(char));
                    strcat(infix, *tokenizedString);
                    strcat(infix, " ");
                    infix[strlen(infix)] = '\0';
                    postfix = realloc(postfix, (strlen(postfix) + strlen(*tokenizedString) + 1) * sizeof(char));
                    strcat(postfix, " ");
                }

                //if we have reached the end of the expression
                if(tokenPosition == tokenNumber - 1){
                    convertToPostfixPeep(infix, postfix);
                    postfix[strlen(postfix)] = '\0';
                    int count = evaluatePostfixExpressionPeep(HMLMemory, SymbolTable, postfix,
                                            instructionCounterPtr, flags, loc);
                    (*symbolCounterPtr) += count;
                }
            }

            //if we hit an if statement
            if(tokenValue == IF){
                branch = true;
            }

            if(branch == true){
                if(tokenValue == VAR || tokenValue == -VAR || tokenValue == -CONST || tokenValue == CONST){
                    if(op1 == -1){
                        HMLMemory[*instructionCounterPtr] = 0x100 * LOAD;
                        (*instructionCounterPtr)++;

                        HMLMemory[*instructionCounterPtr] = 0x100 * SUB;
                        (*instructionCounterPtr)++;

                        op1 = SymbolTable[*tableIndexPtr].location;
                    }
                    else{
                        op2 = SymbolTable[*tableIndexPtr].location;
                    }
                }

                switch(tokenValue){
                    case 8:
                        HMLMemory[*instructionCounterPtr] = 0x100 * BNEG;
                        break;
                    case 9: 
                        lessEqual = true;
                        HMLMemory[*instructionCounterPtr] = 0x100 * BNEG;
                        (*instructionCounterPtr)++;
                        HMLMemory[*instructionCounterPtr] = 0x100 * BZRO;
                        break;
                    case 10:
                        HMLMemory[*instructionCounterPtr] = 0x100 * BZRO;
                        break;
                    case 11:
                        greaterEqual = true;
                        HMLMemory[*instructionCounterPtr] = 0x100 * BPOS;
                        (*instructionCounterPtr)++;
                        HMLMemory[*instructionCounterPtr] = 0x100 * BZRO;
                        break;
                    case 12:
                        HMLMemory[*instructionCounterPtr] = 0x100 * BPOS;
                        break;
                    case 16:
                        notEqual = true;
                        HMLMemory[*instructionCounterPtr] = 0x100 * BNEG;
                        (*instructionCounterPtr)++;
                        HMLMemory[*instructionCounterPtr] = 0x100 * BPOS;
                        break;
                }
            }

            //if the current Peep statement is a goto
            if(tokenValue == GOTO){
                goTo = true;
            }

            //if we have encountered the line number in a goto statement
            if(goTo == true && (tokenValue == LINE || tokenValue == -LINE)){

                if(branch == true){
                    if(lessEqual == true || greaterEqual == true || notEqual == true){
                        HMLMemory[*instructionCounterPtr - 3] += op1;
                        HMLMemory[*instructionCounterPtr - 2] += op2;

                        if(!searchTable(SymbolTable, LINE, *tokenizedString, &memoryLocationPtr)){
                            flags[*instructionCounterPtr - 1] = atoi(*tokenizedString);
                            flags[*instructionCounterPtr] = atoi(*tokenizedString);
                        }

                        else{
                            HMLMemory[*instructionCounterPtr] += SymbolTable[*repeatValuePtr].location;
                        }
                        (*instructionCounterPtr)++;
                    }
                    else{
                        HMLMemory[*instructionCounterPtr - 2] += op1;
                        HMLMemory[*instructionCounterPtr - 1] += op2;

                        if(!searchTable(SymbolTable, LINE, *tokenizedString, &memoryLocationPtr)){
                            flags[*instructionCounterPtr] = atoi(*tokenizedString);
                        }
                        else{
                            HMLMemory[*instructionCounterPtr] += SymbolTable[*repeatValuePtr].location;
                        }
                        (*instructionCounterPtr)++;
                    }
                }
                else{
                    HMLMemory[*instructionCounterPtr] = 0x100 * B;

                    if(!searchTable(SymbolTable, LINE, *tokenizedString, &memoryLocationPtr)){
                        flags[*instructionCounterPtr] = atoi(*tokenizedString);
                    }
                    else{
                        HMLMemory[*instructionCounterPtr] += SymbolTable[*repeatValuePtr].location;
                    }
                    (*instructionCounterPtr)++;
                }
            }

            tokenizedString++;
            tokenPosition++;
        }
    }
}

void secondPass(unsigned int HMLMemory[], TableEntry SymbolTable[], int flags[]){
    printSymbolTable(SymbolTable);
    resolveReferences(flags, HMLMemory, SymbolTable);
    saveFile(HMLMemory);
}

void printSymbolTable(TableEntry SymbolTable[]){
    printf("SYMBOL TABLE: \n");
    for(int i = 0; i<256; i++){
        if(SymbolTable[i].isEmpty == true){
            break;
        }
        if(SymbolTable[i].type == 'V'){
            char sym = SymbolTable[i].symbol;
            char typ = SymbolTable[i].type;
            int loc = SymbolTable[i].location;
            printf("Symbol: %c, Type: %c, Location: 0x%02X\n", sym, typ, loc);
        }
        else{
            int sym = SymbolTable[i].symbol;
            char typ = SymbolTable[i].type;
            int loc = SymbolTable[i].location;
            printf("Symbol: %d, Type: %c, Location: 0x%02X\n", sym, typ, loc);
        }
    }
}

char **tokenize(char **tokenizedString, char *peepStatement){

    char **temp_string = tokenizedString;
    unsigned int tokens = 0; //number of processed tokens

    //tokenize first segment
    char *token_ptr = strtok(peepStatement, " ");

    temp_string = calloc(tokens + 1, sizeof(char*));
        
    while(token_ptr != NULL){
        temp_string[tokens] = token_ptr;
        tokens++;
        temp_string = realloc(temp_string, (tokens + 1) * sizeof(char *));
        token_ptr = strtok(NULL, " ");
    }

    temp_string[tokens] = NULL;

    tokenizedString = calloc(tokens + 1, sizeof(char *));

    tokenizedString = &temp_string[0];

    return tokenizedString;
}

unsigned int getTokenType(char *token, unsigned int tokenPosition, TableEntry SymbolTable[],
                            int **memoryLocationPtr, unsigned int *repeatValuePtr, bool goTo){
        
    char token1[20];
    strcpy(token1, token);
    removeChar(token1, '(');
    removeChar(token1, ')');
    
    if(!strcmp(token1, "rem")){
        return REM;
    }

    else if(tokenPosition != 0){
        if(goTo == false){
            if(atoi(token1)){
                if((*repeatValuePtr = searchTable(SymbolTable, CONST, token1, memoryLocationPtr))){
                                return -CONST;
                    }
                    else{
                        return CONST;
                    }
                }
            }
            else{
                if((*repeatValuePtr = searchTable(SymbolTable, LINE, token, memoryLocationPtr))){
                    *repeatValuePtr = searchTable(SymbolTable, LINE, token, memoryLocationPtr);
                    return -LINE;
                }
                else{
                    return LINE;
                }
            }
        }
        if(strlen(token) == 1){
            if(*token >= 97 && *token <= 122){
                if((*repeatValuePtr = searchTable(SymbolTable, VAR, token1, memoryLocationPtr))){
                    return -VAR;
                }
                else{
                    return VAR;
                }
            }
        }
        if(tokenPosition == 0){
            if((*repeatValuePtr = searchTable(SymbolTable, LINE, token, memoryLocationPtr))){
                return -LINE;
            }
            else{
                return LINE;
            }
        }
        if(!strcmp(token, "print")){
            return PRINT;
        }
        if(!strcmp(token, "input")){
            return INPUT;
        }
        if(!strcmp(token, "if")){
            return IF;
        }
        if(!strcmp(token, "let")){
            return LET;
        }
        if(!strcmp(token, "<")){
            return LESSTHAN;
        }
        if(!strcmp(token, "<=")){
            return LESSEQUAL;
        }
        if(!strcmp(token, "==")){
            return EQUAL;
        }
        if(!strcmp(token, ">=")){
            return GREATEREQUAL;
        }
        if(!strcmp(token, ">")){
            return GREATERTHAN;
        }
        if(!strcmp(token, "!=")){
            return NOTEQUAL;
        }
        if(!strcmp(token, "goto")){
            return GOTO;
        }
        if(!strcmp(token, "end")){
            return END;
        }
        return ERR;
}

//searches the symbol table for the symbol indicated by char *token
int searchTable(TableEntry SymbolTable[], unsigned int symCode, char *token, int **memoryLocationPtr){
    unsigned int count = 0;

    while(count < 256){
        switch(symCode){
            case 1:
                if(SymbolTable[count].type == 'C'){
                    if(atoi(token) == SymbolTable[count].symbol){
                        *memoryLocationPtr = &SymbolTable[count].location;
                        return count;
                    }
                }
                break;
            case 2:
                if(token[0] == SymbolTable[count].symbol){
                    *memoryLocationPtr = &SymbolTable[count].location;
                    return count;
                }
                break;
            case 3:
                if(SymbolTable[count].type == 'L'){
                    if(atoi(token) == SymbolTable[count].symbol){
                        *memoryLocationPtr = &SymbolTable[count].location;
                        return count;
                    }
                }
                break;
        }
        count++;
    }
    return 0;
}

//adds symbol to the symbol table
unsigned int addSymbol(unsigned int tokenValue, char *token, TableEntry SymbolTable[], unsigned int *symbolCounterPtr, 
                        unsigned int *lineCounterPtr, bool goTo){
    
    unsigned int count;

    for(count = 0; count < 256; count++){
        if(SymbolTable[count].isEmpty){

            if(tokenValue == VAR){
                SymbolTable[count].symbol = token[0];
                SymbolTable[count].type = 'V';
                SymbolTable[count].location = 256 - *symbolCounterPtr - 1;
                SymbolTable[count].isEmpty = false;
                break;
            }

            if(tokenValue == CONST){
                if(atoi(token) == 0 && strcmp(token, "0")){
                    break;
                }
                SymbolTable[count].symbol = atoi(token);
                SymbolTable[count].type = 'C';
                SymbolTable[count].location = 256 - *symbolCounterPtr - 1;
                SymbolTable[count].isEmpty = false;
                break;
            }

            if(tokenValue == LINE){
                if(goTo == false){
                    if(!strcmp(token, "00")){
                        SymbolTable[count].symbol = 00;
                    }
                    else{
                        SymbolTable[count].symbol = atoi(token);
                    }
                    SymbolTable[count].type = 'L';
                    SymbolTable[count].location = *lineCounterPtr;
                    SymbolTable[count].isEmpty = false;
                    break;
                }
                else{
                    return count;
                }
            }
        }
    }
    return count;
}

//resolves unresolved references for the second pass
void resolveReferences(int flags[], unsigned int HMLMemory[], TableEntry SymbolTable[]){

    for(int i = 0; i < 256; i++){
        if(flags[i] != -1 && flags[i] != -2){
            for(int j = 0; j < 256; j++){
                if(SymbolTable[j].symbol == flags[i] && SymbolTable[j].type == 'L'){
                    HMLMemory[i] += SymbolTable[j].location;
                }
            }
        }
    }
}

//writes the HML compiler output to "output.hml"
void saveFile(unsigned int HMLMemory[]){
    FILE *cOutput;

    if((cOutput = fopen("output.hml", "w+")) != NULL){
        for(int i = 0; i < 256; i++){
            fprintf(cOutput, "%04X\n", HMLMemory[i]);
        }
        fclose(cOutput);
    }
    else{
        printf("%s\n", "Unable to save compiler output file");
    }
}

//ported version of the infix-to-postfix algorithm for the compiler
void convertToPostfixPeep(char infix[], char postfix[]){

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
        else if(isalpha(infix[i])){
            postfix[j] = infix[i];
            j++;
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

//ported version of the Postfix Evaluation algorithm that handles variables
int evaluatePostfixExpressionPeep(unsigned int HMLMemory[], TableEntry SymbolTable[],
                                    char postfix[], unsigned int *instructionCounterPtr, int flags[], int loc){
    StackNodePtr topNodePtr;
    topNodePtr = NULL; 
    unsigned int i = 0; 
    unsigned int x, y; 
    int memoryLocation = 0; 
    int *memoryLocationPtr; 
    memoryLocationPtr = &memoryLocation;
    int count = 0;

    char str[2]; 

    while (postfix[i] != '\0'){
        str[0] = postfix[i];
        str[1] = '\0';
        
        if(str[0] == ' '){

        }

        else if(isdigit(str[0])){

            int num = 0;
            while(postfix[i] != '\0' && isdigit(postfix[i])){
                num = num * 10 + postfix[i] - '0';
                i++;
            }
            char snum[5];
            sprintf(snum, "%d", num);
            if(searchTable(SymbolTable, CONST, snum, &memoryLocationPtr)){
                pushInt(&topNodePtr, *memoryLocationPtr);
            }
        }
        else if(isalpha(str[0]) && searchTable(SymbolTable, VAR, &str[0], &memoryLocationPtr)) {
            pushInt(&topNodePtr, *memoryLocationPtr);
        }
   
        else if(isOperator(str[0])) {
            x = (unsigned)popInt(&topNodePtr);
            y = (unsigned)popInt(&topNodePtr);
            int result = loadInstruction(HMLMemory, x, y, postfix[i], instructionCounterPtr, flags, loc);
            count++;
            pushInt(&topNodePtr, result);
        }
        i++;
    }
    return count; 
}

//helper function for postfix evaluation that adds the appropriate HML instructions to memory
//for expression operand1 operator operand2
int loadInstruction(unsigned int HMLMemory[], unsigned int operand1, unsigned int operand2, char operator1,
                                unsigned int *instructionCounterPtr, int flags[], int loc){
  
    int result = 0;
    HMLMemory[*instructionCounterPtr] = 0x100 * LOAD;
    HMLMemory[*instructionCounterPtr] += operand2;
    (*instructionCounterPtr)++;

    switch(operator1) {
    
        case '+': 
            HMLMemory[*instructionCounterPtr] = 0x100 * ADD;
            HMLMemory[*instructionCounterPtr] += operand1;
            break;
        case '-': 
            HMLMemory[*instructionCounterPtr] = 0x100 * SUB;
            HMLMemory[*instructionCounterPtr] += operand1;
            break;
        case '*':  
            HMLMemory[*instructionCounterPtr] = 0x100 * MUL;
            HMLMemory[*instructionCounterPtr] += operand1;
            break;
        case '/': 
            HMLMemory[*instructionCounterPtr] = 0x100 * DIV;
            HMLMemory[*instructionCounterPtr] += operand1;
            break;
        case '%': 
            HMLMemory[*instructionCounterPtr] = 0x100 * MOD;
            HMLMemory[*instructionCounterPtr] += operand1;
            break;
        default:
            break;
    }
    (*instructionCounterPtr)++;
 
    HMLMemory[*instructionCounterPtr] = 0x100 * STORE;
    HMLMemory[*instructionCounterPtr] += nextEmpty(HMLMemory, flags); 
    (*instructionCounterPtr)++;
    HMLMemory[*instructionCounterPtr] = 0x100 * LOAD;
    HMLMemory[*instructionCounterPtr] += nextEmpty(HMLMemory, flags); 
    (*instructionCounterPtr)++;
    result = nextEmpty(HMLMemory, flags);
    flags[nextEmpty(HMLMemory, flags)] = -2;
    HMLMemory[*instructionCounterPtr] = 0x100 * STORE;
    HMLMemory[*instructionCounterPtr] += loc;
    (*instructionCounterPtr)++;
    return result;
}

//helper for loadInstruction to determine location in HML memory of 
//temporary values computed during expression evaluation
int nextEmpty(unsigned int HMLMemory[], int flags[]){
    int ind = 0;
    for(int i = 255; i>=0; i--){
        if(HMLMemory[i] == 0 && flags[i] != -2){
            ind = i;
            break;
        }
    }
    return ind;
}
void removeChar(char* s, char c){
 
    int j, n = strlen(s);
    for (int i = j = 0; i < n; i++)
        if (s[i] != c)
            s[j++] = s[i];
 
    s[j] = '\0';
}
//================================================
// main function
//================================================
int main(int argc, char **argv){

    if(argv[1] != NULL){
        FILE *peepFile;
        if((peepFile = fopen(argv[1], "r+")) != NULL){
            char *dot = strrchr(argv[1], '.');
            if (dot && !strcmp(dot, ".peep")){
                compile(peepFile);
                fclose(peepFile);
                printf("%s\n", "Peep file compiled! See output.hml for HML code");
            }
            else{
                printf("%s\n", "Please enter a .peep file");
                fclose(peepFile);
            }
        }
        else{
            printf("%s\n", "Please enter a valid .peep filepath");
        }
    }
    else{
        printf("%s\n", "Usage: ./peepcc <peep file path>");
    }
    return (0);
}