//Group 47
//Pulkit Gupta - 2019B3A70481P
//Aishwarya Sam - 2020A7PS0001P
//Rohan Kunwar - 2019B3A70666P
//Amaan Zaffar - 2019B3A70463P

struct Token{
    int lineNumber;
    char tokenID[20];
    char  lexeme[25];
};

struct FileHandle{
    int lineNumber;
    FILE* fp;
    char buffer[25];
    int bufferSize;
    bool isComment;
    bool commentFreeMode;
};

struct LookupNode{
    char pattern[25];
    char token[25];
};