//Group 47
//Pulkit Gupta - 2019B3A70481P
//Aishwarya Sam - 2020A7PS0001P
//Rohan Kunwar - 2019B3A70666P
//Amaan Zaffar - 2019B3A70463P

#include "stackDef.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

//Character Stack
void stackPush(struct Stack* s, char* element){
    if(s -> top == NULL){
        s -> top = (struct Node*) malloc(sizeof(struct Node));
        strcpy(s -> top -> element, element);
    }
    else{
        struct Node* temp = s -> top;
        s -> top = (struct Node*) malloc(sizeof(struct Node));
        strcpy(s -> top -> element, element);
        s -> top -> next = temp;
    }
}

bool stackIsEmpty(struct Stack* s){
    if(s -> top == NULL){
        return true;
    }
    return false;
}

void stackPop(struct Stack* s){
    if(!stackIsEmpty(s)){
        struct Node* temp = s -> top;
        s -> top = s -> top -> next;
        //free(temp);
    }
    else{
        printf("Error: Parsing Stack Empty\n");
    }
}

char* stackTop(struct Stack* s){
    if(!stackIsEmpty(s)){
        return s -> top -> element;
    }
    printf("Error: Parsing Stack Empty\n");
    return "";
}

//Parse Tree Stack
void parseTreeStackPush(struct ParseTreeStack* s, struct ParseTreeNode* element){
    if(s -> top == NULL){
        s -> top = (struct ParseTreeStackNode*) malloc(sizeof(struct ParseTreeStackNode));
        s -> top -> parseTreeNode = element;
    }
    else{
        struct ParseTreeStackNode* temp = s -> top;
        s -> top = (struct ParseTreeStackNode*) malloc(sizeof(struct ParseTreeStackNode));
        s -> top -> parseTreeNode = element;
        s -> top -> next = temp;
    }
}

bool parseTreeStackIsEmpty(struct ParseTreeStack* s){
    if(s -> top == NULL){
        return true;
    }
    return false;
}

void parseTreeStackPop(struct ParseTreeStack* s){
    if(!parseTreeStackIsEmpty(s)){
        struct ParseTreeStackNode* temp = s -> top;
        s -> top = s -> top -> next;
        //free(temp);
    }
    else{
        printf("Error: Parsing Stack Empty\n");
    }
}

struct ParseTreeNode* parseTreeStackTop(struct ParseTreeStack* s){
    if(!parseTreeStackIsEmpty(s)){
        return s -> top -> parseTreeNode;
    }
    printf("Error: Parsing Stack Empty\n");
    return NULL;
}