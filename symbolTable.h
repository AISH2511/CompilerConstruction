#include "parserDef.h"

void initializeSymbolTable();
int indexInSymbolTable(char* moduleName);
void insertDeclaredModules(struct ParseTreeNode* root);
void insertOtherModules(struct ParseTreeNode* root);
void printSymbolTable();
void insertDriverModule();
void insertAllDeclare(struct ParseTreeNode* root);
void setOffset();
void printActivationRecords();
void printArrayVariables();
void detectSemanticErrors(struct ParseTreeNode* root);
