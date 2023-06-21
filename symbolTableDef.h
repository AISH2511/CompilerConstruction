struct TableNode{
    bool isInserted;
    bool wasDeclared;
    struct List_syn_node* input_plist;
    struct List_syn_node* output_plist;
    struct TableListNode* variableList;
};

struct TableListNode{
    char name[30];
    char scope[30];
    int scopeStart;
    int scopeEnd;
    int lineNumber;
    char type[20];
    bool is_array;
    char arrayRange1[10];
    char arrayRange2[10];
    char staticOrDynamic[10];
    int width;
    int offset;
    int nestingLevel;
    struct TableListNode* next;
};