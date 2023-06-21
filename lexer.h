//Group 47
//Pulkit Gupta - 2019B3A70481P
//Aishwarya Sam - 2020A7PS0001P
//Rohan Kunwar - 2019B3A70666P
//Amaan Zaffar - 2019B3A70463P

void lookupInsert(char* data[2]);
char* lookupSearch(char* key);
void createLookupTable();
struct Token createToken(struct FileHandle* fileHandle, char* tokenID);
struct Token reportLexicalError(struct FileHandle* fileHandle);
char readChar(struct FileHandle* fileHandle);
void retract(struct FileHandle* fileHandle, int num);
struct Token getNextToken(struct FileHandle* fileHandle);