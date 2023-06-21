//Group 47
//Pulkit Gupta - 2019B3A70481P
//Aishwarya Sam - 2020A7PS0001P
//Rohan Kunwar - 2019B3A70666P
//Amaan Zaffar - 2019B3A70463P

#include <stdbool.h>
#include "stackDef.h"

void initializeParsing();
void storeGrammar();
void calculateFirstAndFollows();
int createParseTable();
int parseToken(struct Stack* s, struct Token token);
void addToParseTree(struct ParseTreeStack* s, struct Token* token);
struct ParseTreeNode* createParseTreeNode(char* symbol, struct ParseTreeNode* parent);
struct ParseTreeNode* createParseTreeLeaf(struct Token* token, struct ParseTreeNode* parent);
void printParseTree(struct ParseTreeNode* node);