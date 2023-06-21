//Group 47
//Pulkit Gupta - 2019B3A70481P
//Aishwarya Sam - 2020A7PS0001P
//Rohan Kunwar - 2019B3A70666P
//Amaan Zaffar - 2019B3A70463P

struct GrammarListNode{
    bool isTerminal;
    char symbol[100];
    struct GrammarListNode* next;
};

struct GrammarNode{
    char symbol[100];
    struct GrammarListNode* grammarRuleList[10];
    struct GrammarListNode* firstSet;
    struct GrammarListNode* followSet;
    bool calculatedFirstSet;
    bool calculatedFollowSet;
};

struct ParseTreeNode{
    bool isLeaf;
    struct Token* token;
    char symbol[100];
    struct ParseTreeListNode* children;
    struct ParseTreeNode* parent;
    struct ParseTreeNode* addr_syn;
    struct List_syn_node* list_syn;
    char type[15];
    bool isArray;
    bool decError;
};

struct List_syn_node{
    struct ParseTreeNode* node;
    struct List_syn_node* next;
};

struct ParseTreeListNode{
    struct ParseTreeNode* element;
    struct ParseTreeListNode* next;
};