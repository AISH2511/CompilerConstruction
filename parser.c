//Group 47
//Pulkit Gupta - 2019B3A70481P
//Aishwarya Sam - 2020A7PS0001P
//Rohan Kunwar - 2019B3A70666P
//Amaan Zaffar - 2019B3A70463P

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "stackDef.h"
#include "parserDef.h"
#include "lexerDef.h"
#include "stack.h"

//This is the variable which stores all the Grammar rules, first sets and follow sets
int SIZE = 200;
struct GrammarNode* grammar[200];
char* startSymbol = "program";
char terminals[100][30];
char nonTerminals[100][30];
int numTerminals = 0;
int numNonTerminals = 0;
struct GrammarListNode* parseTable[100][100];
bool panicMode = false;

//Function to print a particular set
void printSet(struct GrammarListNode* setNode){
    while(setNode != NULL){
        printf("%s ", setNode -> symbol);
        setNode = setNode -> next;
    }
}
//Function to check if a given key belongs to a set
bool setContainsKey(struct GrammarListNode* setNode, char* key){
    if(setNode == NULL){
        return false;
    }
    else if(!strcmp(setNode -> symbol, key)){
        return true;
    }
    else return setContainsKey(setNode -> next, key);
}

//Function to insert a key in a set (Usage -> setNode = setInsertKey(setNode, key);) 
struct GrammarListNode* setInsertKey(struct GrammarListNode* setNode, char* key){
    if(setNode == NULL){
        setNode = (struct GrammarListNode*) malloc(sizeof(struct GrammarListNode));
        strcpy(setNode -> symbol, key);
        return setNode;
    }
    else if(strlen(setNode -> symbol) == 0){
        strcpy(setNode -> symbol, key);
        return setNode;
    }
    else if(setContainsKey(setNode, key)){
        return setNode;
    }
    struct GrammarListNode* temp = setNode;
    while(temp -> next != NULL){
        temp = temp -> next;
    }
    temp -> next = (struct GrammarListNode*) malloc(sizeof(struct GrammarListNode));
    strcpy(temp -> next -> symbol, key);
    return setNode;
}

//Function to add the contents of set2 to set1
void unionSet(struct GrammarListNode* set1, struct GrammarListNode* set2){
    while(set2 != NULL){
        set1 = setInsertKey(set1, set2 -> symbol);
        set2 = set2 -> next;
    }
}

//Union without epsilon(#)
void unionSetWithoutEpsilon(struct GrammarListNode* set1, struct GrammarListNode* set2){
    while(set2 != NULL){
        if(strcmp(set2 -> symbol, "#")){
            set1 = setInsertKey(set1, set2 -> symbol);
        }
        set2 = set2 -> next;
    }
}

//Function to add a particular rule to our grammar
void insertRule(char* rule){
    char* token = strtok(rule, " ");
    int hash = (token[0] - 'A') % SIZE;
    while(grammar[hash] != NULL){
        if(!strcmp(grammar[hash] -> symbol, token)){
            break;
        }
        hash++;
        hash %= SIZE;
    }
    if(grammar[hash] == NULL){
        grammar[hash] = (struct GrammarNode*)malloc(sizeof(struct GrammarNode));
        strcpy(grammar[hash] -> symbol, token);
    }

    int ind = 0;
    while(grammar[hash] -> grammarRuleList[ind] != NULL){
        ind++;
    }
    token = strtok(NULL, " ");
    grammar[hash] -> grammarRuleList[ind] = (struct GrammarListNode*) malloc(sizeof(struct GrammarListNode));
    strcpy(grammar[hash] -> grammarRuleList[ind] -> symbol, token);
    if((token[0] <= 'z' && token[0] >= 'a')){ 
        grammar[hash] -> grammarRuleList[ind] -> isTerminal = false;
    }
    else{
        grammar[hash] -> grammarRuleList[ind] -> isTerminal = true;
    }
    token = strtok(NULL, " ");
    struct GrammarListNode* temp = grammar[hash] -> grammarRuleList[ind];
    while(token != NULL){
        temp -> next = (struct GrammarListNode*) malloc(sizeof(struct GrammarListNode));
        strcpy(temp -> next -> symbol, token);
        if(token[0] <= 'z' && token[0] >= 'a'){
            temp -> next -> isTerminal = false;
        }
        else{
            temp -> next -> isTerminal = true;
        }
        token = strtok(NULL, " ");
        temp = temp -> next;
    }
}

//Function to return the index of a Non Terminal in the grammar variable
int searchGrammarTable(char* token){
    for(int i = 0; i < SIZE; i++){
        if(grammar[i] != NULL && !strcmp(token, grammar[i] -> symbol)){
            return i;
        }
    }
}

int searchTerminal(char* symbol){
    for(int i = 0; i < (sizeof(nonTerminals) / sizeof(nonTerminals[0])); i++){
        if(!strcmp(symbol, terminals[i])){
            return i;
        }
    }
    printf("Error finding terminal in terminals\n");
}

int searchNonTerminal(char* symbol){
    for(int i = 0; i < (sizeof(nonTerminals) / sizeof(nonTerminals[0])); i++){
        if(!strcmp(symbol, nonTerminals[i])){
            return i;
        }
    }
    printf("Error finding terminal in terminals\n");
}

//Function to calculate the first set of a non terminal
struct GrammarListNode* calculateFirstSet(int ind){
    if(grammar[ind] -> calculatedFirstSet == true){
        return grammar[ind] -> firstSet;
    }
    for(int i = 0; i < 10; i++){
        if(grammar[ind] -> grammarRuleList[i] == NULL){
            break;
        }
        struct GrammarListNode* temp = grammar[ind] -> grammarRuleList[i];
        while(temp != NULL){
            if(temp -> isTerminal){
                grammar[ind] -> firstSet = setInsertKey(grammar[ind] -> firstSet, temp -> symbol);
                break;
            }
            else{
                int index = searchGrammarTable(temp -> symbol);
                grammar[index] -> firstSet = calculateFirstSet(index);
                grammar[index] -> calculatedFirstSet = true;
                unionSetWithoutEpsilon(grammar[ind] -> firstSet, grammar[index] -> firstSet);
                if(!setContainsKey(grammar[index] -> firstSet, "#")){
                    break;
                }
                temp = temp -> next;
            }
        }
        if(temp == NULL){
            grammar[ind] -> firstSet = setInsertKey(grammar[ind] -> firstSet, "#");
        }
    }
    grammar[ind] -> calculatedFirstSet = true;
    return grammar[ind] -> firstSet;
}

//Function to calculate the follow set of a non terminal
void calculateFollowSet(int ind){
    for(int i = 0; i < 10; i++){
        if(grammar[ind] -> grammarRuleList[i] == NULL){
            break;
        }
        struct GrammarListNode* temp = grammar[ind] -> grammarRuleList[i];
        while(temp != NULL){
            if(temp -> isTerminal){
                temp = temp -> next;
                continue;
            }
            else{
                if(temp -> next != NULL){
                    if(temp -> next -> isTerminal && strcmp(temp -> next -> symbol, "#")){
                        setInsertKey(grammar[searchGrammarTable(temp -> symbol)] -> followSet, temp -> next -> symbol);
                    }
                    else{
                        struct GrammarListNode* temp2 = temp -> next;
                        while(temp2 != NULL){
                            if(temp2 -> isTerminal && strcmp(temp2 -> symbol, "#")){
                                setInsertKey(grammar[searchGrammarTable(temp -> symbol)] -> followSet, temp2 -> symbol);
                                break;
                            }
                            unionSetWithoutEpsilon(grammar[searchGrammarTable(temp -> symbol)] -> followSet, grammar[searchGrammarTable(temp2 -> symbol)] -> firstSet);
                            if(!setContainsKey(grammar[searchGrammarTable(temp2 -> symbol)] -> firstSet, "#")){
                                break;
                            }
                            temp2 = temp2 -> next;
                        }
                        if(temp2 == NULL){
                            unionSetWithoutEpsilon(grammar[searchGrammarTable(temp -> symbol)] -> followSet, grammar[ind] -> followSet);
                        }
                    }
                }
                else{
                    unionSetWithoutEpsilon(grammar[searchGrammarTable(temp -> symbol)] -> followSet, grammar[ind] -> followSet);
                }
                temp = temp -> next;
            }
        }
    }
}

//Function to store terminals and non terminals
void storeListOfTerms(){
    FILE* fp = fopen("terminals.txt", "r");
    int ind = 0;
    if(fp == NULL){
        printf("Error: terminals.txt file not found\n");
    }
    char buffer[100];
    while(!feof(fp)){
        memset(buffer, '\0', sizeof(buffer));
        fgets(buffer, 100, fp);
        buffer[strcspn(buffer, "\n\r ")] = 0;
        strcpy(terminals[ind], buffer);
        ind++;
    }

    fp = fopen("nonTerminals.txt", "r");
    if(fp == NULL){
        printf("Error: nonTerminals.txt file not found\n");
    }
    ind = 0;
    while(!feof(fp)){
        memset(buffer, '\0', sizeof(buffer));
        fgets(buffer, 100, fp);
        buffer[strcspn(buffer, "\n\r ")] = 0;
        strcpy(nonTerminals[ind], buffer);
        ind++;
    }
}

void addToParseTable(int ind, int rule){
    struct GrammarListNode* firstSet = (struct GrammarListNode*) malloc(sizeof(struct GrammarListNode));
    struct GrammarListNode* temp = grammar[ind] -> grammarRuleList[rule];
    while(temp != NULL){
        if(strcmp("#", temp -> symbol) && temp -> isTerminal){
            setInsertKey(firstSet, temp -> symbol);
            break;
        }
        if(strcmp("#", temp -> symbol)){
            unionSetWithoutEpsilon(firstSet, grammar[searchGrammarTable(temp -> symbol)] -> firstSet);
            if(!setContainsKey(grammar[searchGrammarTable(temp -> symbol)] -> firstSet, "#")){
                break;
            }
        }
        temp = temp -> next;
    }
    if(temp == NULL){
        unionSet(firstSet, grammar[ind] -> followSet);
        if(setContainsKey(firstSet, "$")){
            struct GrammarListNode* newRule = (struct GrammarListNode*) malloc(sizeof(struct GrammarListNode));
            setInsertKey(newRule, "#");
            parseTable[ind][0] = newRule;
        }
    }
    temp = firstSet;
    int i = 0, j = 0;
    for(i = 0; i < numNonTerminals; i++){
        if(!strcmp(nonTerminals[i], grammar[ind] -> symbol)){
            break;
        }
    }
    while (temp != NULL)
    {
        for(j = 0; j < numTerminals; j++){
            if(!strcmp(temp -> symbol, terminals[j])){
                break;
            }
        }
        parseTable[i][j] = grammar[ind] -> grammarRuleList[rule];
        temp = temp -> next;
    }    
}

//Function to create the parse table
void storeParseTable(){
    for(int i = 0; i < numNonTerminals; i++){
        int ind = searchGrammarTable(nonTerminals[i]);
        for(int j = 0; j < 10; j++){
            if(grammar[ind] -> grammarRuleList[j] == NULL){
                break;
            }
            addToParseTable(ind, j);
        }
    }
}

int initializeParsing(){
    //Counting the number of terminals  
    FILE* fp = fopen("terminals.txt", "r");
    if(fp == NULL){
        printf("Error: terminals.txt file not found\n");
        return 1;
    }
    char buffer[100];
    while(!feof(fp)){
        memset(buffer, '\0', sizeof(buffer));
        fgets(buffer, 100, fp);
        numTerminals++;
    }

    //Counting the number of non terminals
    fp = fopen("nonTerminals.txt", "r");
    if(fp == NULL){
        printf("Error: nonTerminals.txt file not found\n");
        return 1;
    }
    while(!feof(fp)){
        memset(buffer, '\0', sizeof(buffer));
        fgets(buffer, 100, fp);
        numNonTerminals++;
    }
}

int storeGrammar(){
    //Opening the grammar file
    char* grammarFilename = "grammar.txt";
    FILE* fp= fopen(grammarFilename, "r");
    char buffer[100];
    if(fp == NULL){
        printf("Error: Grammar file not found\n");
        return 1;
    }

    //Storing the Grammar Rules
    while(!feof(fp)){
        memset(buffer, '\0', sizeof(buffer));
        fgets(buffer, 100, fp);
        buffer[strcspn(buffer, "\n\r")] = 0;
        if(strcspn(buffer,"\0") == 0){
            continue;
        }
        insertRule(buffer);
    }
}

void calculateFirstAndFollows(){
    //Initializing first and follow calculations
    for(int i = 0; i < SIZE; i++){
        if(grammar[i] != NULL){
            grammar[i] -> calculatedFirstSet = false;
            grammar[i] -> calculatedFollowSet = false;
            grammar[i] -> firstSet = (struct GrammarListNode*) malloc(sizeof(struct GrammarListNode));
            grammar[i] -> followSet = (struct GrammarListNode*) malloc(sizeof(struct GrammarListNode));
        }
    }

    //Calculating first set
    for(int i = 0; i < SIZE; i++){
        if(grammar[i] != NULL){
            grammar[i] -> firstSet = calculateFirstSet(i);
        }
    }

    //Calculating follow set
    strcpy(grammar[searchGrammarTable(startSymbol)] -> followSet -> symbol, "$");
    for(int i = 0; i < 100; i++){
        for(int j = 0; j < SIZE; j++){
            if(grammar[j] != NULL){
                calculateFollowSet(j);
            }
        }
    }
}

void createParseTable(){
    //Creating the parse table
    storeListOfTerms();
    storeParseTable();
}

int parseToken(struct Stack* s, struct Token token){
    int errors = 0;
    if(panicMode){
        if(setContainsKey(grammar[searchGrammarTable(stackTop(s))] -> followSet, token.tokenID)){
            stackPop(s);
            panicMode = false;
        }
        else{
            return errors;
        }
    }
    while(!stackIsEmpty(s)){
        if(stackTop(s)[0] >= 'a' && stackTop(s)[0] <= 'z'){
            int i = searchNonTerminal(stackTop(s));
            int j = searchTerminal(token.tokenID);
            if(parseTable[i][j] == NULL){
                errors++;
                printf("Syntax Error: Unexpected expression %s on line number %d\n", token.lexeme, token.lineNumber);
                panicMode = true;
                break;
            }
            else{
                stackPop(s);
                struct GrammarListNode* temp = parseTable[i][j];
                if(strcmp(temp -> symbol, "#")){
                    struct Stack* tempStack = (struct Stack*) malloc(sizeof(struct Stack));
                    while(temp != NULL){
                        stackPush(tempStack, temp -> symbol);
                        temp = temp -> next;
                    }
                    while (!stackIsEmpty(tempStack)){
                        stackPush(s, stackTop(tempStack));
                        stackPop(tempStack);
                    }
                }
            }
        }
        else{
            if(!(strcmp(stackTop(s), token.tokenID))){
                stackPop(s);
                break;
            }
            else{
                printf("Syntax Error: Expected expression %s but found %s on line number %d\n", stackTop(s), token.lexeme, token.lineNumber);
                errors++;
                stackPop(s);
            }

        }
    }
    return errors;
}

void insertChild(struct ParseTreeNode* parent, struct ParseTreeNode* child){
    if(parent -> children == NULL){
        parent -> children = (struct ParseTreeListNode*) malloc(sizeof(struct ParseTreeListNode));
        parent -> children -> element = child;
        parent -> children -> next = NULL;
    }
    else{
        struct ParseTreeListNode* temp = parent -> children;
        while(temp -> next != NULL){
            temp = temp -> next;
        }
        temp -> next = (struct ParseTreeListNode*) malloc(sizeof(struct ParseTreeListNode));
        temp -> next -> element = child;
        temp -> next -> next = NULL;
    }
}

struct ParseTreeNode* createParseTreeNode(char* symbol, struct ParseTreeNode* parent){
    struct ParseTreeNode* temp = (struct ParseTreeNode*) malloc(sizeof(struct ParseTreeNode));
    temp -> isLeaf = false;
    temp -> parent = parent;
    strcpy(temp -> symbol, symbol);
    return temp;
}

struct ParseTreeNode* createParseTreeLeaf(struct Token* token, struct ParseTreeNode* parent){
    struct ParseTreeNode* temp = (struct ParseTreeNode*) malloc(sizeof(struct ParseTreeNode));
    temp -> isLeaf = true;
    temp -> parent = parent;
    temp -> token = token;
    if(token != NULL){
        strcpy(temp -> symbol, token -> tokenID);
    }
    return temp;
}

void addToParseTree(struct ParseTreeStack* s, struct Token* token){
    while(!parseTreeStackIsEmpty(s)){
        struct ParseTreeNode* topNode = parseTreeStackTop(s);
        if(topNode -> symbol[0] >= 'a' && topNode -> symbol[0] <= 'z'){
            int i = searchNonTerminal(parseTreeStackTop(s) -> symbol);
            int j = searchTerminal(token -> tokenID);
            parseTreeStackPop(s);
            struct GrammarListNode* temp = parseTable[i][j];
            if(strcmp(temp -> symbol, "#")){
                struct ParseTreeStack* tempStack = (struct ParseTreeStack*) malloc(sizeof(struct ParseTreeStack));
                while(temp != NULL){
                    struct ParseTreeNode* tempNode;
                    if(temp -> symbol[0] >= 'a' && temp -> symbol[0] <= 'z'){
                        tempNode = createParseTreeNode(temp -> symbol, topNode);
                    }
                    else{
                        tempNode = createParseTreeLeaf(NULL, topNode);
                        strcpy(tempNode -> symbol, temp -> symbol);
                    }
                    insertChild(topNode, tempNode);
                    parseTreeStackPush(tempStack, tempNode);
                    temp = temp -> next;
                }
                while (!parseTreeStackIsEmpty(tempStack)){
                    parseTreeStackPush(s, parseTreeStackTop(tempStack));
                    parseTreeStackPop(tempStack);
                }
            }
            else{
                insertChild(topNode, createParseTreeNode("#", topNode));
            }
        }
        else{
            parseTreeStackTop(s) -> token = token; 
            parseTreeStackPop(s);
            break;
        }
    }
}

void printParseTree(struct ParseTreeNode* node){
    if(node == NULL){
        return;
    }
    if(node -> isLeaf || !strcmp(node -> symbol, "#")){
        if(!strcmp(node -> symbol, "#")){
            printf("%-25s%-25s%-10s%-25s%-25s%-25s%-25s%-25s\n", "#", node -> symbol, "----", "#", 
            "----", node -> parent -> symbol, "yes", node -> symbol);
        }
        else{
            printf("%-25s%-25s%-10d%-25s%-25s%-25s%-25s%-25s\n", node -> token -> lexeme, node -> symbol, node -> token -> lineNumber, node -> token -> tokenID, 
            (!strcmp(node -> symbol, "NUM") || !strcmp(node -> symbol, "RNUM")?node -> token -> lexeme:"----"), node -> parent -> symbol, "yes", node -> symbol);
        }
    }
    else{
        printParseTree(node -> children -> element);
        printf("%-25s%-25s%-10s%-25s%-25s%-25s%-25s%-25s\n", "----", node -> symbol, "----", node -> symbol, 
            "----", node -> parent == NULL?"----":node->parent -> symbol, "no", node -> symbol);
        struct ParseTreeListNode* temp = node -> children -> next;
        while(temp != NULL){
            printParseTree(temp -> element);
            temp = temp -> next;
        }
    }
}

//To iterate over the Grammar table
    // for(int i = 0; i < SIZE; i++){
    //     if(grammar[i] != NULL){
    //         for(int j = 0; j < 10; j++){
    //             if(grammar[i] -> grammarRuleList[j] != NULL){
    //                 printf("%s ", grammar[i] -> symbol);
    //                 printSet(grammar[i] -> grammarRuleList[j]);
    //                 printf("\n");
    //             }
    //         }
    //     }
    // }
    