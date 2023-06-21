//Group 47
//Pulkit Gupta - 2019B3A70481P
//Aishwarya Sam - 2020A7PS0001P
//Rohan Kunwar - 2019B3A70666P
//Amaan Zaffar - 2019B3A70463P

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lexerDef.h"

//Set of keywords and tokens
char keywords[30][2][100] = {
        {"integer", "INTEGER"},
        {"real", "REAL"},
        {"boolean", "BOOLEAN"},
        {"of", "OF"},
        {"array", "ARRAY"},
        {"start", "START"},
        {"end", "END"},
        {"declare", "DECLARE"},
        {"module", "MODULE"},
        {"driver", "DRIVER"},
        {"program", "PROGRAM"},
        {"get_value", "GET_VALUE"},
        {"print", "PRINT"},
        {"use", "USE"},
        {"with", "WITH"},
        {"parameters", "PARAMETERS"},
        {"takes", "TAKES"},
        {"input", "INPUT"},
        {"returns", "RETURNS"},
        {"for", "FOR"},
        {"in", "IN"},
        {"switch", "SWITCH"},
        {"case", "CASE"},
        {"break", "BREAK"},
        {"default", "DEFAULT"},
        {"while", "WHILE"},
        {"AND", "AND"},
        {"OR", "OR"},
        {"true", "TRUE"},
        {"false", "FALSE"}
    };

//Everything related to Lookup Table
struct LookupNode* lookupTable[60];

void lookupInsert(char data[2][100]){
    int hash = data[0][0] - 'A';
    while(1){
        if(lookupTable[hash] == NULL){
            lookupTable[hash] = (struct LookupNode*)malloc(sizeof(struct LookupNode));
            strcpy(lookupTable[hash] -> pattern, data[0]);
            strcpy(lookupTable[hash] -> token, data[1]);
            return;
        }
        hash++;
        hash %= sizeof(lookupTable) / sizeof(lookupTable[0]);
    }
}

char* lookupSearch(char* key){
    int hash = key[0] - 'A';
    while (lookupTable[hash] != NULL){
        if(!strcmp(lookupTable[hash] -> pattern, key)){
            return lookupTable[hash] -> token;
        }
        hash++;
        hash %= sizeof(lookupTable) / sizeof(lookupTable[0]);
    }
    return "";
}

void createLookupTable(){
    for(int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++){
        lookupInsert(keywords[i]);
    }
}

//Function for creating a token
struct Token createToken(struct FileHandle* fileHandle, char* tokenID){
    struct Token token;
    strcpy(token.tokenID, tokenID);
    token.lineNumber = fileHandle -> lineNumber;
    strcpy(token.lexeme, fileHandle -> buffer);
    return token;
}

//Function for reporting lexical error
struct Token reportLexicalError(struct FileHandle* fileHandle){
    struct Token token;
    if(fileHandle -> buffer[0] == EOF){
        strcpy(token.tokenID, "$");
        strcpy(token.lexeme, "EOF");
        token.lineNumber = -2;
    }
    else if(fileHandle -> bufferSize >= 21){
        if(fileHandle -> isComment == false && fileHandle -> commentFreeMode == false){
            printf("Lexical Error: Expression %s... exceeds the size limit on line number %d\n", fileHandle -> buffer, fileHandle -> lineNumber);
        }
        token.lineNumber = -1;
    }
    else{
        if(fileHandle -> isComment == false && fileHandle -> commentFreeMode == false){
            printf("Lexical Error: Unidentified expression %s on line number %d\n", fileHandle -> buffer, fileHandle -> lineNumber);
        }
        token.lineNumber = -1;
    }
    return token;
}

//Function for reading a single character from file and adding it to the buffer and incrementing buffer size
char readChar(struct FileHandle* fileHandle){
    char ch = fgetc(fileHandle -> fp);
    fileHandle -> buffer[(fileHandle -> bufferSize)] = ch;
    fileHandle -> bufferSize++;
    return ch;
}

//Function for retracting file pointer any number of times
void retract(struct FileHandle* fileHandle, int num){
    for(int i = 0; i < num; i++){
        (fileHandle -> bufferSize)--;
        fileHandle -> buffer[(fileHandle -> bufferSize)] = '\0';
    }
    if(!feof(fileHandle -> fp)){
        fseek(fileHandle -> fp, -num, SEEK_CUR);
    }
    else{
        fseek(fileHandle -> fp, -num + 1, SEEK_CUR);
    }
}

struct Token getNextToken(struct FileHandle* fileHandle){
    //Code for getting the token
    char ch;
    int error = 0;
    do{
        ch = fgetc(fileHandle -> fp);

        //Skipping initial white spaces and incrementing the line number on encountering a new line
        if(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r'){
            if(ch == '\n'){
                (fileHandle -> lineNumber)++;
            }
            continue;
        }
        fileHandle -> buffer[fileHandle -> bufferSize] = ch;
        fileHandle -> bufferSize++;

        //Checking for character patterns
        if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch == '_')){
            do{
                ch = readChar(fileHandle);
            }while(fileHandle -> bufferSize <= 21 && (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch == '_') || (ch >= '0' && ch <= '9'));

            while((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch == '_') || (ch >= '0' && ch <= '9')){
                ch = fgetc(fileHandle -> fp);
            }
            retract(fileHandle, 1);
            if(fileHandle -> bufferSize >= 21){
                return reportLexicalError(fileHandle);
            }
            char *tokenID = lookupSearch(fileHandle -> buffer);
            if(strcmp(tokenID, "")){
                return createToken(fileHandle, tokenID);
            }
            return createToken(fileHandle, "ID");
        }


        //Checking for number patterns
        if(ch >= '0' && ch <= '9'){
            do{
                ch = readChar(fileHandle);
            }while(ch >= '0' && ch <= '9');

            if(ch == '.'){
                ch = readChar(fileHandle);
                if(ch == '.'){
                    retract(fileHandle, 2);
                    return createToken(fileHandle, "NUM");
                }
                else if(ch >= '0' && ch <= '9'){
                    do{
                        ch = readChar(fileHandle);
                    }while(ch >= '0' && ch <= '9');
                    
                    if(ch == 'E' || ch == 'e'){
                        ch = readChar(fileHandle);
                        if(ch == '+' || ch == '-'){
                            ch = readChar(fileHandle);
                            if(ch >= '0' && ch <= '9'){
                                do{
                                    ch = readChar(fileHandle);
                                }
                                while(ch >= '0' && ch <= '9');
                                retract(fileHandle, 1);
                                return createToken(fileHandle, "RNUM");
                                
                            }
                            else{
                                retract(fileHandle, 1);
                                return reportLexicalError(fileHandle);
                            }

                        }
                        else if((ch >= '0' && ch <= '9')){
                            do{
                                ch = readChar(fileHandle);
                            }while(ch >= '0' && ch <= '9');
                            retract(fileHandle, 1);
                            return createToken(fileHandle, "RNUM");
                        }
                        else{
                            retract(fileHandle, 1);
                            return reportLexicalError(fileHandle);
                        }
                    }
                    else{
                        retract(fileHandle, 1);
                        return createToken(fileHandle, "RNUM");
                    }
                }
                else{
                    retract(fileHandle, 1);
                    return reportLexicalError(fileHandle);
                }
            }
            else{
                retract(fileHandle, 1);
                return createToken(fileHandle, "NUM");
            }
        }

        //Checking for other patterns
        switch(ch){
            case '[':
                return createToken(fileHandle, "SQBO");
            case ']':
                return createToken(fileHandle, "SQBC");
            case '(':
                return createToken(fileHandle, "BO");
            case ')':
                return createToken(fileHandle, "BC");
            case ';':
                return createToken(fileHandle, "SEMICOL");
            case ',':
                return createToken(fileHandle, "COMMA");
            case '+':
                return createToken(fileHandle, "PLUS");
            case '-':
                return createToken(fileHandle, "MINUS");
            case '/':
                return createToken(fileHandle, "DIV");
            case '!':
                ch = readChar(fileHandle);
                switch(ch){
                    case '=':
                        return createToken(fileHandle, "NE");
                    default:
                        retract(fileHandle, 1);
                        return reportLexicalError(fileHandle);
                }
            case '*':
                ch = readChar(fileHandle);
                switch(ch){
                    case '*':
                        fileHandle -> isComment = !fileHandle -> isComment;
                        return createToken(fileHandle, "COMMENTMARK");
                    default:
                        retract(fileHandle, 1);
                        return createToken(fileHandle, "MUL");
                }
            case ':':
                ch = readChar(fileHandle);
                switch(ch){
                    case '=':
                        return createToken(fileHandle, "ASSIGNOP");
                    default:
                        retract(fileHandle, 1);
                        return createToken(fileHandle, "COLON");
                }
            case '=':
                ch = readChar(fileHandle);
                switch(ch){
                    case '=':
                        return createToken(fileHandle, "EQ");
                    default:
                        retract(fileHandle, 1);
                        return reportLexicalError(fileHandle);
                }
            case '.':
                ch = readChar(fileHandle);
                switch(ch){
                    case '.':
                        return createToken(fileHandle, "RANGEOP");
                    default:
                        retract(fileHandle, 1);
                        return reportLexicalError(fileHandle);
                }
            case '<':
                ch = readChar(fileHandle);
                switch(ch){
                    case '=':
                        return createToken(fileHandle, "LE");
                    case '<':
                        ch = readChar(fileHandle);
                        switch(ch){
                            case '<':
                                return createToken(fileHandle, "DRIVERDEF");
                            default:
                                retract(fileHandle, 1);
                                return createToken(fileHandle, "DEF");
                        }
                    default:
                        retract(fileHandle, 1);
                        return createToken(fileHandle, "LT");
                }
            case '>':
                ch = readChar(fileHandle);
                switch(ch){
                    case '=':
                        return createToken(fileHandle, "GE");
                    case '>':
                        ch = readChar(fileHandle);
                        switch(ch){
                            case '>':
                                return createToken(fileHandle, "DRIVERENDDEF");
                            default:
                                retract(fileHandle, 1);
                                return createToken(fileHandle, "ENDDEF");
                        }
                    default:
                        retract(fileHandle, 1);
                        return createToken(fileHandle, "GT");
                }
            default:
                return reportLexicalError(fileHandle);
        }
    }while(ch != EOF);
}