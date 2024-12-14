#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void lexer(char* input);
void parse();
void expression();
void term();
void factor();

char tokens[100][20];  
int tokenCount = 0;    
int currentToken = 0;  
char intermediateCode[100][50];
int tempVarCounter = 0;

int hasError = 0;

int main() {
    char input[100];

    printf("Enter an arithmetic expression (e.g., x = a + b * (c - d);):\n");
    fgets(input, 100, stdin);
    input[strcspn(input, "\n")] = 0; 

 
    tokenCount = 0;
    currentToken = 0;
    tempVarCounter = 0;
    hasError = 0;


    lexer(input);

    if (!hasError) {
        parse();

        if (!hasError) {
            printf("\nThree-Address Code (TAC):\n");
            for (int i = 0; i < tempVarCounter; i++) {
                printf("%s\n", intermediateCode[i]);
            }
        }
    }

    return 0;
}

void lexer(char* input) {
    int i = 0;

    while (input[i] != '\0') {
        if (isspace(input[i])) {
            i++;
            continue;
        }

        if (isalpha(input[i])) {
            char identifier[20];
            int k = 0;
            while (isalnum(input[i])) {
                identifier[k++] = input[i++];
            }
            identifier[k] = '\0';
            strcpy(tokens[tokenCount++], identifier);
        }
        else if (isdigit(input[i])) {
            char number[20];
            int k = 0;
            while (isdigit(input[i])) {
                number[k++] = input[i++];
            }
            number[k] = '\0';
            strcpy(tokens[tokenCount++], number);
        }
        else if (strchr("+-*/=();", input[i])) {
            char operator[2] = {input[i], '\0'};
            strcpy(tokens[tokenCount++], operator);
            i++;
        }
        else {
            printf("Error: Invalid character '%c'\n", input[i]);
            hasError = 1;
            return;
        }
    }

    strcpy(tokens[tokenCount++], "EOF");
}

void parse() {
    if (strcmp(tokens[currentToken], "EOF") == 0) {
        printf("Error: Empty input.\n");
        hasError = 1;
        return;
    }

    char variable[20];
    if (isalpha(tokens[currentToken][0])) {
        strcpy(variable, tokens[currentToken]);
        currentToken++;

        if (strcmp(tokens[currentToken], "=") == 0) {
            currentToken++;
            expression();
            if (strcmp(tokens[currentToken], ";") == 0) {
                currentToken++;
                sprintf(intermediateCode[tempVarCounter], "%s = t%d", variable, tempVarCounter - 1);
                tempVarCounter++;
            } else {
                printf("Error: Missing semicolon.\n");
                hasError = 1;
            }
        } else {
            printf("Error: Missing '=' after variable.\n");
            hasError = 1;
        }
    } else {
        printf("Error: Invalid assignment statement.\n");
        hasError = 1;
    }
}

void expression() {
    term();
    while (strcmp(tokens[currentToken], "+") == 0 || strcmp(tokens[currentToken], "-") == 0) {
        char operator[2];
        strcpy(operator, tokens[currentToken]);
        currentToken++;
        
        int leftTemp = tempVarCounter - 1;  
        term();
        
        
        sprintf(intermediateCode[tempVarCounter], "t%d = t%d %s t%d", 
                tempVarCounter, leftTemp, operator, tempVarCounter - 1);
        tempVarCounter++;
    }
}

void term() {
    factor();
    while (strcmp(tokens[currentToken], "*") == 0 || strcmp(tokens[currentToken], "/") == 0) {
        char operator[2];
        strcpy(operator, tokens[currentToken]);
        currentToken++;
        
        int leftTemp = tempVarCounter - 1;  
        factor();
        
        sprintf(intermediateCode[tempVarCounter], "t%d = t%d %s t%d", 
                tempVarCounter, leftTemp, operator, tempVarCounter - 1);
        tempVarCounter++;
    }
}

void factor() {
    if (isalpha(tokens[currentToken][0]) || isdigit(tokens[currentToken][0])) {
        sprintf(intermediateCode[tempVarCounter], "t%d = %s", tempVarCounter, tokens[currentToken]);
        tempVarCounter++;
        currentToken++;
    } else if (strcmp(tokens[currentToken], "(") == 0) {
        currentToken++;
        expression();
        if (strcmp(tokens[currentToken], ")") == 0) {
            currentToken++;
        } else {
            printf("Error: Missing closing parenthesis.\n");
            hasError = 1;
        }
    } else {
        printf("Error: Invalid factor '%s'\n", tokens[currentToken]);
        hasError = 1;
    }
}