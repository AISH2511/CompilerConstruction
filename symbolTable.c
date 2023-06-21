#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "lexerDef.h"
#include "parserDef.h"
#include "symbolTableDef.h"

int TABLESIZE = 1000;
int nestingLevel = 0;
struct TableNode* symbolTable[1000];
char moduleIndex[1000][30];
struct Token* tk;

void initializeSymbolTable(){
    for(int i = 0; i < TABLESIZE; i++){
        symbolTable[i] = NULL;
        strcpy(moduleIndex[i], "");
    }
}

void printSymbolTable(){
    printf("%-25s%-25s%s\t%-10s\t%-10s\t%-15s%s\t%s\t%s\t%s\n",
    "Variable Name", "Scope(Module)", "Scope(LineNo.)", "Type", "isArray?", "ArrayType", "ArrayRange", "Width", "Offset", "Nesting Level");
    for(int i = 0; i < TABLESIZE; i++){
        if(symbolTable[i] == NULL){
            break;
        }
        struct TableListNode* temp = symbolTable[i] -> variableList;
        while(temp != NULL){
            if(temp -> is_array){
                printf("%-25s%-25s[%d-%d]\t\t%-10s\t%-10s\t%-15s[%s-%s]\t\t%-5d\t%-5d\t%-5d\n", 
                temp -> name, temp -> scope, temp -> scopeStart, temp -> scopeEnd, temp -> type, temp -> is_array?
                 "yes":"no", temp -> staticOrDynamic, temp -> arrayRange1, temp -> arrayRange2, temp -> width, temp -> offset, temp -> nestingLevel);
            }
            else{
                printf("%-25s%-25s[%d-%d]\t\t%-10s\t%-10s\t%-15s[%s-%s]\t\t%-5d\t%-5d\t%-5d\n", 
                temp -> name, temp -> scope, temp -> scopeStart, temp -> scopeEnd, temp -> type, temp -> is_array?
                "yes":"no", "----", "-", "--", temp -> width, temp -> offset, temp -> nestingLevel);
            }
            temp = temp -> next;
        }
    }
}

int indexInSymbolTable(char* moduleName){
    for(int i = 0; i < TABLESIZE; i++){
        if(!strcmp(moduleName, moduleIndex[i])){
            return i;
        }
        else if(!strcmp(moduleIndex[i], "")){
            return -1;
        }
    }
    return -1;
}

int nextFreeIndex(){
    for(int i = 0; i < TABLESIZE; i++){
        if(!strcmp(moduleIndex[i], "")){
            return i;
        }
    }
    printf("Symbol Table Size Exceeded\n");
    return -1;
}

void insertDeclaredModules(struct ParseTreeNode* root){
    struct List_syn_node* temp = root -> list_syn;
    while(temp != NULL && temp -> node != NULL){
        int ind = nextFreeIndex();
        strcpy(moduleIndex[ind], temp -> node -> token -> lexeme);
        symbolTable[ind] = (struct TableNode*) malloc(sizeof(struct TableNode));
        symbolTable[ind] -> wasDeclared = true;
        temp = temp -> next;
    }
}

int checkIfAlreadyDeclared(struct TableListNode* node, int ind){
    for(struct TableListNode* temp = symbolTable[ind] -> variableList; temp != NULL; temp = temp -> next){
        if(!strcmp(temp -> name, node -> name) && (temp -> nestingLevel == node -> nestingLevel) && (temp -> scopeStart == node -> scopeStart) && (temp -> scopeEnd == node -> scopeEnd)){
            printf("Semantic Error: Redeclartion of variable %s on line number %d\n", node -> name, node -> lineNumber);
            return true;
        }
    }
    return false;
}

void insertIDInSymbolTable(struct ParseTreeNode* node ,int ind, struct ParseTreeNode* dataType){
    struct TableListNode* temp = (struct TableListNode*) malloc(sizeof(struct TableListNode));
    strcpy(temp -> name, node -> token -> lexeme);
    strcpy(temp -> scope, moduleIndex[ind]);
    struct ParseTreeNode* scopeCheck = node;
    while(1){
        if(!strcmp(scopeCheck -> symbol, "module")){
            temp -> scopeStart = scopeCheck -> children -> element -> token -> lineNumber;
            temp -> scopeEnd = scopeCheck -> children -> next -> next -> next -> next -> next -> next -> next -> next -> next -> next -> next -> element -> children -> next -> next -> element -> token -> lineNumber;
            break;
        }
        else if(!strcmp(scopeCheck -> symbol, "driverModule")){
            temp -> scopeStart = scopeCheck -> children -> element -> token -> lineNumber;
            temp -> scopeEnd = scopeCheck -> children -> next -> next -> next -> next -> element -> children -> next -> next -> element -> token -> lineNumber;
            break;
        }
        else if(!strcmp(scopeCheck -> symbol, "statement") && !strcmp(scopeCheck -> list_syn -> node -> symbol, "FOR")){
            scopeCheck = scopeCheck -> children -> element;
            temp -> scopeStart = scopeCheck -> children -> element -> token -> lineNumber;
            temp -> scopeEnd = scopeCheck -> children -> next -> next -> next -> next -> next -> next -> next -> next -> element -> token -> lineNumber;
            break;
        }
        else if(!strcmp(scopeCheck -> symbol, "statement") && !strcmp(scopeCheck -> list_syn -> node -> symbol, "WHILE")){
            scopeCheck = scopeCheck -> children -> element;
            temp -> scopeStart = scopeCheck -> children -> element -> token -> lineNumber;
            temp -> scopeEnd = scopeCheck -> children -> next -> next -> next -> next -> next -> next -> element -> token -> lineNumber;
            break;
        }
        else if(!strcmp(scopeCheck -> symbol, "caseStmts") || !strcmp(scopeCheck -> symbol, "n9")){
            temp -> scopeStart = scopeCheck -> children -> element -> token -> lineNumber;
            temp -> scopeEnd = scopeCheck -> children -> next -> next -> next -> next -> next -> element -> token -> lineNumber;
            break;
        }
        else if(!strcmp(scopeCheck -> symbol, "default")){
            temp -> scopeStart = scopeCheck -> children -> element -> token -> lineNumber;
            temp -> scopeEnd = scopeCheck -> children -> next -> next -> next -> next -> element -> token -> lineNumber;
            break;
        }
        scopeCheck = scopeCheck -> parent;
    }
    temp -> lineNumber = node -> token -> lineNumber;
    if(!strcmp(dataType -> symbol, "type")){
        strcpy(temp -> type, dataType -> addr_syn -> symbol);
        temp -> is_array = false;
        if(!strcmp(temp -> type, "INTEGER"))
        {
            temp -> width = 2;
        }
        else if(!strcmp(temp -> type, "REAL")){
            temp -> width = 4;
        }
        else{
            temp -> width = 1;
        }
    }
    else{
        if(dataType -> addr_syn != NULL){
            strcpy(temp -> type, dataType -> addr_syn -> symbol);
            if(!strcmp(temp -> type, "INTEGER"))
            {
                temp -> width = 2;
            }
            else if(!strcmp(temp -> type, "REAL")){
                temp -> width = 4;
            }
            else{
                temp -> width = 1;
            }
        }
        else{
            strcpy(temp -> type, dataType -> list_syn -> next -> node -> symbol);
            if(dataType -> list_syn -> node -> list_syn -> node -> list_syn -> node -> addr_syn != NULL){
                temp -> arrayRange1[0] = '-';
                strcat(temp -> arrayRange1, dataType -> list_syn -> node -> list_syn -> node -> list_syn -> next -> node -> addr_syn -> token -> lexeme);
            }
            else{
                strcpy(temp -> arrayRange1, dataType -> list_syn -> node -> list_syn -> node -> list_syn -> next -> node -> addr_syn -> token -> lexeme);
            }
            if(dataType -> list_syn -> node -> list_syn -> next -> node -> list_syn -> node -> addr_syn != NULL){
                temp -> arrayRange2[0] = '-';
                strcat(temp -> arrayRange2, dataType -> list_syn -> node -> list_syn -> next -> node -> list_syn -> next -> node -> addr_syn -> token -> lexeme);
            }
            else{
                strcpy(temp -> arrayRange2, dataType -> list_syn -> node -> list_syn -> next -> node -> list_syn -> next -> node -> addr_syn -> token -> lexeme);
            }
            if(!strcmp(dataType -> list_syn -> node -> list_syn -> next -> node -> list_syn -> next -> node -> addr_syn -> symbol, "ID") || !strcmp(dataType -> list_syn -> node -> list_syn -> node -> list_syn -> next -> node -> addr_syn -> symbol, "ID")){
                strcpy(temp -> staticOrDynamic, "dynamic");
            }
            else{
                int l = atoi(temp -> arrayRange1);
                int r = atoi(temp -> arrayRange2);
                if(!strcmp(temp -> type, "INTEGER"))
                {
                    temp -> width = 2;
                }
                else if(!strcmp(temp -> type, "REAL")){
                    temp -> width = 4;
                }
                else{
                    temp -> width = 1;
                }
                if(r - l >= 0){
                    temp -> width *= (r - l); 
                }
                else{
                    temp -> width *= -(r - l); 
                }
                temp -> width += 3;
                strcpy(temp -> staticOrDynamic, "static");
            }
            temp -> is_array = true;
        }
    }
    temp -> nestingLevel = nestingLevel;
    if(!checkIfAlreadyDeclared(temp, ind)){
        struct TableListNode* temp2 = symbolTable[ind] -> variableList;
        if(temp2 == NULL){
            symbolTable[ind] -> variableList = temp;
        }
        else{
            while(temp2 -> next != NULL){
                temp2 = temp2 -> next;
            }
            temp2 -> next = temp;
        }
    }
}

void insertInputAndOutputList(int ind, struct ParseTreeNode* root){
    struct List_syn_node* temp = root -> list_syn;
    while(temp != NULL && temp -> node != NULL){
        insertIDInSymbolTable(temp -> node, ind, temp -> node -> addr_syn);
        temp = temp -> next;
    }
}

void insertOtherModules(struct ParseTreeNode* root){
    struct List_syn_node* temp = root -> list_syn;
    while(temp != NULL && temp -> node != NULL){
        int ind = indexInSymbolTable(temp -> node -> list_syn -> node -> token -> lexeme);
        if(ind != -1 && symbolTable[ind] -> isInserted == false){
            symbolTable[ind] -> isInserted = true;
            symbolTable[ind] -> input_plist = temp -> node -> list_syn -> next -> node -> list_syn;
            symbolTable[ind] -> output_plist = temp -> node -> list_syn -> next -> next -> node -> list_syn;
            insertInputAndOutputList(ind, temp -> node -> list_syn -> next -> node);
            insertInputAndOutputList(ind, temp -> node -> list_syn -> next -> next -> node);
        }
        else if(ind == -1){
            ind = nextFreeIndex();
            strcpy(moduleIndex[ind], temp -> node -> list_syn -> node -> token -> lexeme);
            symbolTable[ind] = (struct TableNode*) malloc(sizeof(struct TableNode));
            symbolTable[ind] -> isInserted = true;
            symbolTable[ind] -> input_plist = temp -> node -> list_syn -> next -> node -> list_syn;
            symbolTable[ind] -> output_plist = temp -> node -> list_syn -> next -> next -> node -> list_syn;
            insertInputAndOutputList(ind, temp -> node -> list_syn -> next -> node);
            insertInputAndOutputList(ind, temp -> node -> list_syn -> next -> next -> node);
        }
        else{
            printf("Semantic error on line number %d: Redefinition of Module %s\n", temp -> node -> list_syn -> node -> token -> lineNumber, temp -> node -> list_syn -> node -> token -> lexeme);
        }
        temp = temp -> next;
    }
}

void insertDriverModule(){
    int ind = nextFreeIndex();
    strcpy(moduleIndex[ind], "driverModule");
    symbolTable[ind] = (struct TableNode*) malloc(sizeof(struct TableNode));
    symbolTable[ind] -> isInserted = true;
}

void insertAllDeclare(struct ParseTreeNode* root){
    if(!strcmp(root -> symbol, "module") || !strcmp(root -> symbol, "driverModule") || 
    (!strcmp(root -> symbol, "statement") && !strcmp(root -> list_syn -> node -> symbol, "FOR")) ||
    (!strcmp(root -> symbol, "statement") && !strcmp(root -> list_syn -> node -> symbol, "WHILE")) ||
    (!strcmp(root -> symbol, "caseStmts") || !strcmp(root -> symbol, "n9")) || 
    !strcmp(root -> symbol, "default")){
        nestingLevel++;
    }
    else if(!strcmp(root -> symbol, "DECLARE") && strcmp(root -> parent -> symbol, "moduleDeclaration")){
        int ind;
        struct ParseTreeNode* scopeCheck = root;
        while(1){
            if(!strcmp(scopeCheck -> symbol, "module")){
                ind = indexInSymbolTable(scopeCheck -> list_syn -> node -> token -> lexeme);
                break;
            }
            if(!strcmp(scopeCheck -> symbol, "driverModule")){
                ind = indexInSymbolTable("driverModule");
                break;
            }
            scopeCheck = scopeCheck -> parent;
        }
        struct List_syn_node* temp = root -> parent -> list_syn -> next -> node -> list_syn;
        while (temp != NULL && temp -> node != NULL)
        {
            insertIDInSymbolTable(temp -> node, ind, root -> parent -> list_syn -> next -> next -> node);
            temp = temp -> next;
        }
    }
    struct ParseTreeListNode* temp = root -> children;
    while(temp != NULL && temp -> element != NULL){
        insertAllDeclare(temp -> element);
        temp = temp -> next;
    }
    if(!strcmp(root -> symbol, "module") || !strcmp(root -> symbol, "driverModule") || 
    (!strcmp(root -> symbol, "statement") && !strcmp(root -> list_syn -> node -> symbol, "FOR")) ||
    (!strcmp(root -> symbol, "statement") && !strcmp(root -> list_syn -> node -> symbol, "WHILE")) ||
    (!strcmp(root -> symbol, "caseStmts") || !strcmp(root -> symbol, "n9")) || 
    !strcmp(root -> symbol, "default")){
        nestingLevel--;
    }
}

void setOffset(){
    for(int i = 0; i < TABLESIZE; i++){
        if(symbolTable[i] == NULL){
            break;
        }
        else{
            if(symbolTable[i] -> variableList == NULL){
                continue;
            }
            else{
                symbolTable[i] -> variableList -> offset = 0;
                struct TableListNode* temp = symbolTable[i] -> variableList;
                while(temp -> next != NULL){
                    temp -> next -> offset = temp -> offset + temp -> width;
                    temp = temp -> next;
                }
            }
        }
    }
}

void printActivationRecords(){
    for(int i = 0; i < TABLESIZE; i++){
        if(symbolTable[i] == NULL){
            break;
        }
        struct TableListNode* temp = symbolTable[i] -> variableList;
        while(temp != NULL && temp -> next != NULL){
            temp = temp -> next;
        }
        if(temp != NULL){
            printf("%s %d\n", moduleIndex[i], temp -> offset);
        }
    }
}

void printArrayVariables(){
    printf("%-25s%s\t\t%-25s%-15s%-10s\t%-20s\n",
    "Scope(Module)", "Scope(LineNo.)", "Variable Name", "ArrayType", "Type","ArrayRange");
    for(int i = 0; i < TABLESIZE; i++){
        if(symbolTable[i] == NULL){
            break;
        }
        struct TableListNode* temp = symbolTable[i] -> variableList;
        while(temp != NULL){
            if(temp -> is_array){
                printf("%-25s[ %d - %d ]\t\t%-25s%-10s\t%-15s[%s,%s]\n", 
                temp -> scope, temp -> scopeStart, temp -> scopeEnd, temp -> name, temp -> staticOrDynamic, temp -> type,
                temp -> arrayRange1, temp -> arrayRange2);
            }
            temp = temp -> next;
        }
    }
}

struct TableListNode* checkIfDeclared(struct ParseTreeNode* root){
    int ind, scopeStart, scopeEnd;
    struct ParseTreeNode* scopeCheck = root;
    while(1){
        if(!strcmp(scopeCheck -> symbol, "module")){
            ind = indexInSymbolTable(scopeCheck -> list_syn -> node -> token -> lexeme);
            break;
        }
        if(!strcmp(scopeCheck -> symbol, "driverModule")){
            ind = indexInSymbolTable("driverModule");
            break;
        }
        scopeCheck = scopeCheck -> parent;
    }
    scopeCheck = root;
    while(1){
        if(!strcmp(scopeCheck -> symbol, "module")){
            scopeStart = scopeCheck -> children -> element -> token -> lineNumber;
            scopeEnd = scopeCheck -> children -> next -> next -> next -> next -> next -> next -> next -> next -> next -> next -> next -> element -> children -> next -> next -> element -> token -> lineNumber;
            break;
        }
        else if(!strcmp(scopeCheck -> symbol, "driverModule")){
            scopeStart = scopeCheck -> children -> element -> token -> lineNumber;
            scopeEnd = scopeCheck -> children -> next -> next -> next -> next -> element -> children -> next -> next -> element -> token -> lineNumber;
            break;
        }
        else if(!strcmp(scopeCheck -> symbol, "statement") && !strcmp(scopeCheck -> list_syn -> node -> symbol, "FOR")){
            scopeCheck = scopeCheck -> children -> element;
            scopeStart = scopeCheck -> children -> element -> token -> lineNumber;
            scopeEnd = scopeCheck -> children -> next -> next -> next -> next -> next -> next -> next -> next -> element -> token -> lineNumber;
            break;
        }
        else if(!strcmp(scopeCheck -> symbol, "statement") && !strcmp(scopeCheck -> list_syn -> node -> symbol, "WHILE")){
            scopeCheck = scopeCheck -> children -> element;
            scopeStart = scopeCheck -> children -> element -> token -> lineNumber;
            scopeEnd = scopeCheck -> children -> next -> next -> next -> next -> next -> next -> element -> token -> lineNumber;
            break;
        }
        else if(!strcmp(scopeCheck -> symbol, "caseStmts") || !strcmp(scopeCheck -> symbol, "n9")){
            scopeStart = scopeCheck -> children -> element -> token -> lineNumber;
            scopeEnd = scopeCheck -> children -> next -> next -> next -> next -> next -> element -> token -> lineNumber;
            break;
        }
        else if(!strcmp(scopeCheck -> symbol, "default")){
            scopeStart = scopeCheck -> children -> element -> token -> lineNumber;
            scopeEnd = scopeCheck -> children -> next -> next -> next -> next -> element -> token -> lineNumber;
            break;
        }
        scopeCheck = scopeCheck -> parent;
    }
    struct TableListNode* bestNode = NULL;
    for(struct TableListNode* temp = symbolTable[ind] -> variableList; temp != NULL; temp = temp -> next){
        if(!strcmp(temp -> name, root -> token -> lexeme) && (temp -> nestingLevel <= nestingLevel) && (temp -> scopeStart <= scopeStart) && (temp -> scopeEnd >= scopeEnd) && (temp -> lineNumber <= root -> token -> lineNumber)){
            bestNode = temp;
        }
    }
    if(bestNode != NULL){
        return bestNode;
    }
    printf("Semantic Error: Use of undeclared identifier %s on line number %d\n", root -> token -> lexeme, root -> token -> lineNumber);
    return NULL;
}

char* typeCheck(struct ParseTreeNode* root){
    if(root == NULL){
        return "";
    }
    if(!strcmp(root -> symbol, "expression")){
        if(strcmp(root -> addr_syn -> symbol, "u")){
            typeCheck(root -> addr_syn);
            root -> decError = root -> addr_syn -> decError;
            root -> isArray = root -> addr_syn -> isArray;
            strcpy(root -> type, root -> addr_syn -> type);
        }
    }
    if(!strcmp(root -> symbol, "arithmeticOrBooleanExpr")){
        if(root -> list_syn -> next == NULL){
            typeCheck(root -> list_syn -> node);
            root -> decError = root -> list_syn -> node -> decError;
            root -> isArray = root -> list_syn -> node -> isArray;
            strcpy(root -> type, root -> list_syn -> node -> type);
        }
        else{
            for(struct List_syn_node* temp = root -> list_syn; temp != NULL && temp -> node != NULL; temp = temp -> next){
                if(!strcmp(temp -> node -> symbol, "anyTerm")){
                    typeCheck(temp -> node);
                }
            }
            strcpy(root -> type, root -> children -> element -> type);
            root -> isArray = root -> children -> element -> isArray;
            for(struct List_syn_node* temp = root -> list_syn; temp != NULL && temp -> node != NULL; temp = temp -> next){
                if(!strcmp(temp -> node -> symbol, "anyTerm")){
                    if(strcmp(temp -> node -> type, root -> type) || (root -> isArray != temp -> node -> isArray)){
                        strcpy(root -> type, "");
                    }
                    if(temp -> node -> decError){
                        root -> decError = true;
                    }
                }
            }
            if(strcmp(root -> type, "")){
                strcpy(root -> type, "BOOLEAN");
            }
        }
        if(!strcmp(root -> type, "") && !root -> decError){
            printf("Semantic Error: Type Mismatch on Line number %d\n", tk -> lineNumber);
        }
    }
    else if(!strcmp(root -> symbol, "anyTerm")){
        if(root -> children -> next == NULL){
            strcpy(root -> type, "BOOLEAN");
        }
        else{
            if(root -> list_syn -> next == NULL){
                typeCheck(root -> list_syn -> node);
                root -> isArray = root -> list_syn -> node -> isArray;
                root -> decError = root -> list_syn -> node -> decError;
                strcpy(root -> type, root -> list_syn -> node -> type);
            }
            else{
                typeCheck(root -> list_syn -> node);
                typeCheck(root -> list_syn -> next -> node);
                root -> decError = root -> list_syn -> node -> decError || root -> list_syn -> next -> node -> decError;
                if(!strcmp(root -> list_syn -> node -> type, root -> list_syn -> next -> node -> type) && strcmp(root -> list_syn -> node -> type, "") && (root -> list_syn -> node -> isArray == root -> list_syn -> next -> node -> isArray)){
                    strcpy(root -> type, "BOOLEAN");
                }
                else{
                    strcpy(root -> type, "");
                }
            }
        }
    }
    else if(!strcmp(root -> symbol, "arithmeticExpr")){
        for(struct List_syn_node* temp = root -> list_syn; temp != NULL && temp -> node != NULL; temp = temp -> next){
            if(!strcmp(temp -> node -> symbol, "term")){
                typeCheck(temp -> node);
            }
        }
        strcpy(root -> type, root -> children -> element -> type);
        root -> isArray = root -> children -> element -> isArray;
        for(struct List_syn_node* temp = root -> list_syn; temp != NULL && temp -> node != NULL; temp = temp -> next){
            if(!strcmp(temp -> node -> symbol, "term")){
                if(strcmp(temp -> node -> type, root -> type) || (root -> isArray != temp -> node -> isArray)){
                    strcpy(root -> type, "");
                }
                if(temp -> node -> decError){
                    root -> decError = true;
                }
            }
        }
    }
    else if(!strcmp(root -> symbol, "term")){
        for(struct List_syn_node* temp = root -> list_syn; temp != NULL && temp -> node != NULL; temp = temp -> next){
            if(!strcmp(temp -> node -> symbol, "factor")){
                typeCheck(temp -> node);
            }
        }
        bool isDiv = false;
        strcpy(root -> type, root -> children -> element -> type);
        root -> isArray = root -> children -> element -> isArray;
        for(struct List_syn_node* temp = root -> list_syn; temp != NULL && temp -> node != NULL; temp = temp -> next){
            if(!strcmp(temp -> node -> symbol, "factor")){
                if(strcmp(temp -> node -> type, root -> type) || (root -> isArray != temp -> node -> isArray)){
                    strcpy(root -> type, "");
                }
                if(temp -> node -> decError){
                    root -> decError = true;
                }
            }
            else if(!strcmp(temp -> node -> symbol, "DIV")){
                isDiv = true;
            }
        }
        if(isDiv){
            strcpy(root -> type, "REAL");
        }
    }
    else if(!strcmp(root -> symbol, "factor")){
        if(root -> children -> next == NULL){
            tk = root -> children -> element -> token;
            if(!strcmp(root -> children -> element -> symbol, "NUM")){
                root -> isArray = false;
                strcpy(root -> type, "INTEGER");
            }
            else if(!strcmp(root -> children -> element -> symbol, "RNUM")){
                root -> isArray = false;
                strcpy(root -> type, "REAL");
            }
            else if(!strcmp(root -> children -> element -> symbol, "boolConstt")){
                root -> isArray = false;
                strcpy(root -> type, "BOOLEAN");
            }
        }
        else if(root -> children -> next -> next == NULL){
            tk = root -> children -> element -> token;
            struct TableListNode* temp = checkIfDeclared(root -> children -> element);
            if(temp != NULL){
                if(root -> list_syn -> next -> node -> addr_syn == NULL){
                    root -> isArray = temp -> is_array;
                }
                strcpy(root -> type, temp -> type);
            }
            else{
                root -> decError = true;
            }
        }
        else{
            strcpy(root -> type, typeCheck(root -> children -> next -> element));
            root -> isArray = root -> children -> next -> element -> isArray;
            root -> decError = root -> children -> next -> element -> decError;
        }
    }
    return root -> type;
}

void printTypeMismatch(struct ParseTreeNode* root){
    printf("Semantic Error: Type Mismatch of Identifier %s on line number: %d\n", root -> token -> lexeme, root -> token -> lineNumber);
}

void detectSemanticErrors(struct ParseTreeNode* root){
    if(root == NULL){
        return;
    }
    if(!strcmp(root -> symbol, "module") || !strcmp(root -> symbol, "driverModule") || 
    (!strcmp(root -> symbol, "statement") && !strcmp(root -> list_syn -> node -> symbol, "FOR")) ||
    (!strcmp(root -> symbol, "statement") && !strcmp(root -> list_syn -> node -> symbol, "WHILE")) ||
    (!strcmp(root -> symbol, "caseStmts") || !strcmp(root -> symbol, "n9")) || 
    !strcmp(root -> symbol, "default")){
        nestingLevel++;
    }

    for(struct List_syn_node* temp = root -> list_syn; temp != NULL && temp -> node != NULL; temp = temp -> next){
        if(root == temp -> node){
            continue;
        }
        detectSemanticErrors(temp -> node);
    }
    if(root != root -> addr_syn){
        detectSemanticErrors(root -> addr_syn);
    }
    if(!strcmp(root -> symbol, "statement")){
        if(!strcmp(root -> list_syn -> node -> symbol, "GET_VALUE")){
            checkIfDeclared(root -> list_syn -> next -> node);
        }
        else if(!strcmp(root -> list_syn -> node -> symbol, "PRINT")){
            if(root -> list_syn -> next -> node -> addr_syn == NULL){
                struct TableListNode* tableNode = checkIfDeclared(root -> list_syn -> next -> node -> list_syn -> node);
                if(tableNode != NULL){
                    if((!tableNode -> is_array && root -> list_syn -> next -> node -> list_syn -> next -> node -> list_syn != NULL)){
                        printf("Semantic Error: Type Mismatch on line number: %d. Identifier %s is not an array\n",  
                        root -> list_syn -> next -> node -> list_syn -> node -> token -> lineNumber,
                        root -> list_syn -> next -> node -> list_syn -> node -> token -> lexeme);
                    }
                }
            }
        }
        else if(!strcmp(root -> list_syn -> node -> symbol, "ID")){//This is assignment statement which is a child of simple statement
            struct TableListNode* tableNode = checkIfDeclared(root -> list_syn -> node);
            if(tableNode != NULL){
                if(!strcmp(root -> list_syn -> next -> node -> symbol, "expression")){
                        typeCheck(root -> list_syn -> next -> node);
                }
                else if(!strcmp(root -> list_syn -> next -> next -> node -> symbol, "expression")){
                        typeCheck(root -> list_syn -> next -> next -> node);
                }
                else{
                    if(!tableNode -> is_array){
                        printf("Semantic Error: Type Mismatch on line number: %d. Identifier %s is not an array\n",  
                        root -> list_syn -> node -> token -> lineNumber,
                        root -> list_syn -> node -> token -> lexeme);
                    }
                }
            }
        }
        else if(!strcmp(root -> list_syn -> node -> symbol, "optional")){//Module Reuse Statement
            int ind1 = indexInSymbolTable(root -> list_syn -> next -> node -> token -> lexeme), ind;
            if(ind1 == -1){
                printf("Semantic Error: Implicit declaration of module %s on line number %d\n", root -> list_syn -> next -> node -> token -> lexeme, root -> list_syn -> next -> node -> token -> lineNumber);
            }
            else{
                struct ParseTreeNode* scopeCheck = root;
                while(1){
                    if(!strcmp(scopeCheck -> symbol, "module")){
                        ind = indexInSymbolTable(scopeCheck -> list_syn -> node -> token -> lexeme);
                        break;
                    }
                    if(!strcmp(scopeCheck -> symbol, "driverModule")){
                        ind = indexInSymbolTable("driverModule");
                        break;
                    }
                    scopeCheck = scopeCheck -> parent;
                }
                if(ind == ind1){
                    printf("Semantic Error: Illegal function call on line number %d. Recursive call is not allowed\n", root -> list_syn -> next -> node -> token -> lineNumber);
                }
                else if(ind < ind1 && !symbolTable[ind1] -> wasDeclared){
                    printf("Semantic Error: Implicit declaration of module %s on line number %d\n", root -> list_syn -> next -> node -> token -> lexeme, root -> list_syn -> next -> node -> token -> lineNumber);
                }
                else{
                    int numCallInput = 0, numCallOutput = 0, numFuncInput = 0, numFuncOutput = 0;
                    for(struct List_syn_node* temp = root -> list_syn -> node -> list_syn; temp != NULL && temp -> node != NULL; temp = temp -> next){
                        numCallOutput++;
                        checkIfDeclared(temp -> node);
                    }
                    for(struct List_syn_node* temp = root -> list_syn -> next -> next -> node -> list_syn; temp != NULL && temp -> node != NULL; temp = temp -> next -> next){
                        numCallInput++;
                        if(!strcmp(temp -> next -> node -> list_syn -> node -> symbol, "ID")){
                            checkIfDeclared(temp -> next -> node -> list_syn -> node);
                        }
                    }
                    for(struct List_syn_node* temp = symbolTable[ind1] -> input_plist; temp != NULL && temp -> node != NULL; temp = temp -> next){
                        numFuncInput++;
                    }
                    for(struct List_syn_node* temp = symbolTable[ind1] -> output_plist; temp != NULL && temp -> node != NULL; temp = temp -> next){
                        numFuncOutput++;
                    }
                    if(numCallInput != numFuncInput){
                        printf("Semantic Error: number of Input parameters does not match function definition on line number %d\n", root -> list_syn -> next -> node -> token -> lineNumber);
                    }
                    if(numCallOutput != numFuncOutput){
                        printf("Semantic Error: number of Output parameters does not match function definition on line number %d\n", root -> list_syn -> next -> node -> token -> lineNumber);
                    }
                    struct List_syn_node* temp1 = symbolTable[ind1] -> input_plist;
                    for(struct List_syn_node* temp = root -> list_syn -> next -> next -> node -> list_syn; temp != NULL && temp -> node != NULL; temp = temp -> next -> next){
                        if(!strcmp(temp -> next -> node -> list_syn -> node -> symbol, "ID")){
                            struct TableListNode* id1 = checkIfDeclared(temp -> next -> node -> list_syn -> node);
                            struct TableListNode* id2 = checkIfDeclared(temp1 -> node);
                            if(id1 != NULL && id2 != NULL && strcmp(id1 -> type, id2 -> type)){
                                printf("Semantic Error: identifier %s's type does not match function definition on line number %d\n", id1 -> name, root -> list_syn -> next -> node -> token -> lineNumber);
                            }
                        }
                        temp1 = temp1 -> next;
                    }
                    temp1 = symbolTable[ind1] -> output_plist;
                    for(struct List_syn_node* temp = root -> list_syn -> node -> list_syn; temp != NULL && temp -> node != NULL; temp = temp -> next){
                        struct TableListNode* id1 = checkIfDeclared(temp -> node);
                        struct TableListNode* id2 = checkIfDeclared(temp1 -> node);
                        if(id1 != NULL && id2 != NULL && strcmp(id1 -> type, id2 -> type)){
                            printf("Semantic Error: identifier %s's type does not match function definition on line number %d\n", id1 -> name, root -> list_syn -> next -> node -> token -> lineNumber);
                        }
                        temp1 = temp1 -> next;
                    }
                }
            }
        }
        else if(!strcmp(root -> list_syn -> node -> symbol, "FOR")){//For Loop
            struct TableListNode* tableNode = checkIfDeclared(root -> list_syn -> next -> node);
            if(tableNode != NULL && strcmp(tableNode -> type, "INTEGER")){
                printf("Semantic Error: Identifier %s on line number %d must be of type INTEGER\n", tableNode -> name, tableNode -> lineNumber);
            }
        }
        else if(!strcmp(root -> list_syn -> node -> symbol, "WHILE")){//WHILE Loop
            typeCheck(root -> list_syn -> next -> node);
            if(strcmp(root -> list_syn -> next -> node -> type, "")  && !root -> list_syn -> next -> node -> decError){
                printf("Semantic Error: Type Mismatch on line number %d\n", tk -> lineNumber);
            }
        }
    }
    else if(!strcmp(root -> symbol, "var_id_num")){
        if(!strcmp(root -> children -> element -> symbol, "ID")){
            checkIfDeclared(root -> children -> element);
        }
    }
    else if(!strcmp(root -> symbol, "arrFactor")){
        if(!strcmp(root -> children -> element -> symbol, "ID")){
            checkIfDeclared(root -> children -> element);
        }
    }
    // else if(!strcmp(root -> symbol, "expression")){
    //     typeCheck(root);
    // }

    if(!strcmp(root -> symbol, "module") || !strcmp(root -> symbol, "driverModule") || 
    (!strcmp(root -> symbol, "statement") && !strcmp(root -> list_syn -> node -> symbol, "FOR")) ||
    (!strcmp(root -> symbol, "statement") && !strcmp(root -> list_syn -> node -> symbol, "WHILE")) ||
    (!strcmp(root -> symbol, "caseStmts") || !strcmp(root -> symbol, "n9")) || 
    !strcmp(root -> symbol, "default")){
        nestingLevel--;
    }
}