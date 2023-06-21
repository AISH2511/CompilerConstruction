//Group 47
//Pulkit Gupta - 2019B3A70481P
//Aishwarya Sam - 2020A7PS0001P
//Rohan Kunwar - 2019B3A70666P
//Amaan Zaffar - 2019B3A70463P

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "lexerDef.h"
#include "lexer.h"
#include "parser.h"
#include "parserDef.h"
#include "stack.h"
#include "ast.h"
#include "semanticAnalysis.h"

char codeFileName[200];
int numParseNodes = 0;
int numASTnodes = 0;
int printCommentFreeCode(){
    // Opening the code file
    struct FileHandle *fileHandle = (struct FileHandle *)malloc(sizeof(struct FileHandle));
    fileHandle->fp = fopen(codeFileName, "r");
    if (fileHandle->fp == NULL)
    {
        printf("Error: File or Directory not found\n");
        return 1;
    }
    fileHandle->lineNumber = 1;
    fileHandle -> isComment = false;
    fileHandle -> commentFreeMode = true;
    int currentLine = 0;

    // Performing Lexical Analysis
    printf("\n\n\n\n*****************************************Comment Free Code*****************************************\n\n\n\n");
    while (true)
    {
        memset(fileHandle->buffer, '\0', sizeof(fileHandle->buffer));
        fileHandle->bufferSize = 0;
        struct Token token = getNextToken(fileHandle);
        if (token.lineNumber == -2) //-2 means EOF
        {
            break;
        }
        else if(!fileHandle -> isComment || !strcmp(token.tokenID, "COMMENTMARK"))
        {
            if(strcmp(token.tokenID, "COMMENTMARK")){
                if(currentLine != fileHandle -> lineNumber){
                    currentLine = fileHandle -> lineNumber;
                    printf("\n%d) %s ", currentLine, fileHandle -> buffer);
                }
                else{
                    printf("%s ", fileHandle -> buffer);
                }
            }
        }
    }
    fclose(fileHandle->fp);
}

int lexicalAnalysis(){
    // Opening the code file
    struct FileHandle *fileHandle = (struct FileHandle *)malloc(sizeof(struct FileHandle));
    fileHandle->fp = fopen(codeFileName, "r");
    if (fileHandle->fp == NULL)
    {
        printf("Error: File or Directory not found\n");
        return 1;
    }
    fileHandle->lineNumber = 1;
    fileHandle -> isComment = false;
    fileHandle -> commentFreeMode = false;

    // Performing Lexical Analysis
    printf("\n\n\n\n*****************************************Lexical Analysis Output*****************************************\n\n\n\n");
    while (true)
    {
        memset(fileHandle->buffer, '\0', sizeof(fileHandle->buffer));
        fileHandle->bufferSize = 0;
        struct Token token = getNextToken(fileHandle);
        if (token.lineNumber == -2) //-2 means EOF
        {
            break;
        }
        else if (token.lineNumber == -1) //-1 means lexical error
        {
            continue;
        }
        else if(!fileHandle -> isComment || !strcmp(token.tokenID, "COMMENTMARK"))
        {
            printf("%d)\t\t\t\t%s\t\t\t\t%s\n", token.lineNumber, token.lexeme, token.tokenID);
        }
    }
    fclose(fileHandle->fp);
}

struct ParseTreeNode* syntaxAnalysis(){
    // Opening the code file
    struct FileHandle *fileHandle = (struct FileHandle *)malloc(sizeof(struct FileHandle));
    fileHandle->fp = fopen(codeFileName, "r");
    if (fileHandle->fp == NULL)
    {
        printf("Error: File or Directory not found\n");
        return NULL;
    }
    fileHandle->lineNumber = 1;
    fileHandle -> isComment = false;
    fileHandle -> commentFreeMode = false;

    // Starting Parsing
    printf("\n\n\n\n**********************************************************STARTING PARSING*****************************************\n\n\n\n");
    int errors = 0;
    struct Stack *s = (struct Stack *)malloc(sizeof(struct Stack));
    stackPush(s, "$");
    stackPush(s, "program");
    while (!stackIsEmpty(s))
    {
        memset(fileHandle->buffer, '\0', sizeof(fileHandle->buffer));
        fileHandle->bufferSize = 0;
        struct Token token = getNextToken(fileHandle);
        if(fileHandle -> isComment || !strcmp(token.tokenID, "COMMENTMARK")){
            continue;
        }
        else if (token.lineNumber == -2)
        { //-2 means EOF
            token.lineNumber = fileHandle -> lineNumber;
            errors += parseToken(s, token);
            break;
        }
        else if (token.lineNumber == -1)
        { //-1 means lexical error
            errors++;
            continue;
        }
        errors += parseToken(s, token);
    }
    if (errors == 0 && stackIsEmpty(s))
    {
        printf("PARSING SUCCESFUL\n");
    }
    else
    {
        printf("PARSING FAILED\n");
        return NULL;
    }
    fclose(fileHandle->fp);

    //Create Parse Tree 
    if(errors == 0 && stackIsEmpty(s)){
        printf("CREATING PARSE TREE\n");
        // Opening the code file
        fileHandle->fp = fopen(codeFileName, "r");
        fileHandle->lineNumber = 1;
        fileHandle -> isComment = false;
        fileHandle -> commentFreeMode = false;

        // Starting Parsing
        struct ParseTreeNode* parseTreeRoot = createParseTreeNode("program", NULL);
        struct ParseTreeStack *st = (struct ParseTreeStack*) malloc(sizeof(struct ParseTreeStack));
        parseTreeStackPush(st, createParseTreeNode("$", NULL));
        parseTreeStackPush(st, parseTreeRoot);
        while (!parseTreeStackIsEmpty(st))
        {
            memset(fileHandle->buffer, '\0', sizeof(fileHandle->buffer));
            fileHandle->bufferSize = 0;
            struct Token token = getNextToken(fileHandle);
            struct Token* tk = (struct Token*) malloc(sizeof(struct Token));
            tk -> lineNumber = token.lineNumber;
            strcpy(tk -> lexeme, token.lexeme);
            strcpy(tk -> tokenID, token.tokenID);
            if(fileHandle -> isComment || !strcmp(token.tokenID, "COMMENTMARK")){
                continue;
            }
            else if (token.lineNumber == -2)
            { //-2 means EOF
                token.lineNumber = fileHandle -> lineNumber;
                addToParseTree(st, tk);
                break;
            }
            addToParseTree(st, tk);
        }
        printf("PARSE TREE CREATED SUCCESSFULLY\n");
        return parseTreeRoot;
    }
    return NULL;
}

void printAST(struct ParseTreeNode* root){
    if(root == NULL){
        return;
    }
    printf("%-25s\n", root -> symbol);
    if(root -> list_syn != NULL){
        struct List_syn_node* temp = root -> list_syn;
        while(temp != NULL && temp -> node != NULL){
            if(temp -> node == root){
                temp = temp -> next;
                continue;
            }
            printAST(temp -> node);
            temp = temp -> next;
        }
    }
    if(root -> addr_syn != NULL && root -> addr_syn != root){
        printAST(root -> addr_syn);
    }
}

struct ParseTreeNode* timeAnalysis(){
    clock_t start_time, end_time;
    double total_CPU_time, total_CPU_time_in_seconds;
    start_time = clock();
    struct ParseTreeNode* parseTreeRoot = syntaxAnalysis();
    end_time = clock();
    total_CPU_time = (double)(end_time - start_time);
    total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;
    printf("\nTotal time taken to verify the syntactic correctness of the code = %f\n", total_CPU_time_in_seconds);
    return parseTreeRoot;
}

void parseTreeTraversal(struct ParseTreeNode* root){
    if(root == NULL){
        return;
    }
    numParseNodes++;
    for(struct ParseTreeListNode* temp = root -> children; temp != NULL; temp = temp -> next){
        parseTreeTraversal(temp -> element);
    }
}

void astTraversal(struct ParseTreeNode* root){
    if(root == NULL){
        return;
    }
    numASTnodes++;
    for(struct List_syn_node* temp = root -> list_syn; temp != NULL; temp = temp -> next){
        if(root == temp -> node){
            continue;
        }
        astTraversal(temp -> node);
    }
    if(root == root -> addr_syn){
        return;
    }
    astTraversal(root -> addr_syn);
}

void displayMemoryUsed(){
    struct ParseTreeNode* root = syntaxAnalysis();
    numParseNodes = 0;
    numASTnodes = 0;
    parseTreeTraversal(root);
    root = createAST(root);
    astTraversal(root);
    printf("Number of parse tree nodes: %d, Allocated Memmory = %ld Bytes\n", numParseNodes, sizeof(struct ParseTreeNode) * numParseNodes);
    printf("Number of AST nodes: %d, Allocated Memmory = %ld Bytes\n", numASTnodes, sizeof(struct ParseTreeNode) * numASTnodes);
    printf("Compression Percentage = %.2f%%\n", ((numParseNodes - numASTnodes) * 100.0f) / (numParseNodes * 1.0f));
}

void semanticAnalysis(){
    
    clock_t start_time, end_time;
    double total_CPU_time, total_CPU_time_in_seconds;
    start_time = clock();

    struct ParseTreeNode* parseTreeRoot = syntaxAnalysis();
    createAST(parseTreeRoot);
    createSymbolTable(parseTreeRoot);
    checkForSemanticErrors(parseTreeRoot);

    end_time = clock();
    total_CPU_time = (double)(end_time - start_time);
    total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;
    printf("\nTotal time taken to verify the syntactic and semantic correctness of the code\nTotal CPU Time: %.0f clock cycles\nTotal CPU time (in seconds): %.4fs\n",
    total_CPU_time, total_CPU_time_in_seconds);
}

void displaySymbolTable(){
    struct ParseTreeNode* parseTreeRoot = syntaxAnalysis();
    if(parseTreeRoot == NULL){
        return;
    }
    createAST(parseTreeRoot);
    createSymbolTable(parseTreeRoot);
    showSymbolTable();
}

void displayActivationRecords(){
    struct ParseTreeNode* parseTreeRoot = syntaxAnalysis();
    if(parseTreeRoot == NULL){
        return;
    }
    createAST(parseTreeRoot);
    createSymbolTable(parseTreeRoot);
    showActivationRecords();
}

void displayArrayVariables(){
    struct ParseTreeNode* parseTreeRoot = syntaxAnalysis();
    if(parseTreeRoot == NULL){
        return;
    }
    createAST(parseTreeRoot);
    createSymbolTable(parseTreeRoot);
    showArrayVariables();
}

int main(int argc, char* argv[])
{
    // Preprocessing
    if(argc != 4){
        printf("Error: Wrong execution sequence\n");
        return 1;
    }
    strcpy(codeFileName, argv[1]);
    createLookupTable();
    initializeParsing();
    storeGrammar();
    calculateFirstAndFollows();
    createParseTable();

    //Start Compiler
    printf("*******************************************************************\n");
    printf("\tWELCOME TO THE COMPILER OF GROUP 47\n");
    printf("\tImplementation Status:\n");
    printf("\tFIRST and FOLLOW set automated\n");
    printf("\tLexical Analyzer succesfully implemented\n");
    printf("\tSyntax Analyzer succesfully implemented\n");
    printf("\tParse tree could not be constructed\n");
    printf("\tOnly the Grammar rule of moduleReuseStmt creates problem otherwise the whole code is correct\n");
    printf("*******************************************************************\n");
    int choice;
    while (true)
    {
        printf("\n\n\n\nPress 0 to exit\n");
        printf("Press 1 for printing the token list generated by the lexer\n");
        printf("Press 2 for parsing to verify the syntactic correctness of the input source code and to produce parse tree\n");
        printf("Press 3 for printing the Abstract Syntax Tree in appropriate format\n");
        printf("Press 4 for displaying the amount of allocated memory and number of nodes to each of parse tree and abstract syntax tree\n");
        printf("Press 5 for printing the Symbol Table\n");
        printf("Press 6 for displaying Activation record size\n");
        printf("Press 7 for printing the type expressions and width of array variables\n");
        printf("Press 8 for compiling to verify the syntactic and semantic correctness of the input source code\n");
        printf("Prompt: ");
        scanf("%d", &choice);
        if (choice == 0)
            break;
        switch (choice)
        {
            case 1:
                lexicalAnalysis();
                break;
            case 2:
                printf("%-25s%-25s%-10s%-25s%-25s%-25s%-25s%-25s\n","Lexeme", "CurrentNode", "LineNo.", "TokenName", "ValueIfNumber", "ParentNode", "IsLeafNode", "NodeSymbol");
                printParseTree(syntaxAnalysis());
                break;
            case 3:
                printf("Printing AST in Preorder Traversal\n");
                printAST(createAST(syntaxAnalysis()));
                break;
            case 4:
                displayMemoryUsed();
                break;
            case 5:
                displaySymbolTable();
                break;
            case 6:
                displayActivationRecords();
                break;
            case 7:
                displayArrayVariables();
                break;
            case 8:
                semanticAnalysis();
                break;
            default:
                printf("Invalid Choice\n");
        }
    }
}