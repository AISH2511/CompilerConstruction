//Group 47
//Pulkit Gupta - 2019B3A70481P
//Aishwarya Sam - 2020A7PS0001P
//Rohan Kunwar - 2019B3A70666P
//Amaan Zaffar - 2019B3A70463P

struct Node{
    char element[100];
    struct Node* next;
};


struct Stack{
    struct Node* top;
};

struct ParseTreeStackNode{
    struct ParseTreeNode* parseTreeNode;
    struct ParseTreeStackNode* next;
};


struct ParseTreeStack{
    struct ParseTreeStackNode* top;
};