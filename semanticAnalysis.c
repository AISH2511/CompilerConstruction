#include <stdbool.h>
#include <stdio.h>
#include "symbolTable.h"

void createSymbolTable(struct ParseTreeNode* root){
    if(root == NULL){
        return;
    }
    initializeSymbolTable();
    insertDeclaredModules(root -> list_syn -> node);
    insertOtherModules(root -> list_syn -> next -> node);
    insertDriverModule();
    insertOtherModules(root -> list_syn -> next -> next -> next -> node);
    insertAllDeclare(root);
    setOffset();
}

void showSymbolTable(){
    printSymbolTable();
}

void showActivationRecords(){
    printActivationRecords();
}

void showArrayVariables(){
    printArrayVariables();
}

void checkForSemanticErrors(struct ParseTreeNode* root){
    extern int nestingLevel;
    nestingLevel = 0;
    detectSemanticErrors(root);
}