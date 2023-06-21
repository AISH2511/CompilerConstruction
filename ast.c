#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "lexerDef.h"
#include "parserDef.h"

struct List_syn_node* make_list_syn(){
    return (struct List_syn_node*) malloc(sizeof(struct List_syn_node));
}

struct List_syn_node* insert_at_beginning(struct ParseTreeNode* root, struct List_syn_node* list_syn, struct ParseTreeNode* node){
    if(node == NULL){
        return list_syn;
    }
    node -> parent = root;
    if(list_syn != NULL && list_syn -> node == NULL){
        list_syn -> node = node;
        return list_syn;
    }
    else{
        struct List_syn_node* temp = (struct List_syn_node*) malloc(sizeof(struct List_syn_node));;
        temp -> next = list_syn;
        temp -> node = node;
        struct List_syn_node* temp2 = temp;
        while(temp2 != NULL){
            temp2 -> node -> parent = root;
            temp2 = temp2 -> next;
        }
        return temp;
    }
}

void insert_addr(struct ParseTreeNode* root, struct ParseTreeNode* node){
    if(node == NULL){
        return;
    }
    node -> parent = root;
    root -> addr_syn = node;
}

void copy_list_syn(struct ParseTreeNode* root, struct List_syn_node* list_syn){
    if(list_syn != NULL && list_syn -> node != NULL){
        root -> list_syn = list_syn;
        while(list_syn != NULL && list_syn -> node != NULL){
            list_syn -> node -> parent = root;
            list_syn = list_syn -> next;
        }
    }
}

void print_syn_list(struct List_syn_node* list_syn){
    while(list_syn != NULL && list_syn -> node != NULL){
        if(list_syn -> node -> token != NULL){
            printf("%s ", list_syn -> node -> token -> lexeme);
        }
        else{
            printf("%s ", list_syn -> node -> symbol);
        }
        list_syn = list_syn -> next;
    }
    printf("\n");
}

//Functions for implementing semantic rules
void moduleDeclarations(struct ParseTreeNode* root){
    if(!strcmp(root -> children -> element -> symbol, "#")){
        
        root -> list_syn = make_list_syn();
    }
    else{
        root -> list_syn = insert_at_beginning(root, root -> children -> next -> element -> list_syn, root -> children -> element -> addr_syn);
        
    }
}

void moduleDeclaration(struct ParseTreeNode* root){
    insert_addr(root, root -> children -> next -> next -> element);
    
}

void otherModules(struct ParseTreeNode* root){
    if(!strcmp(root -> children -> element -> symbol, "#")){
        
        root -> list_syn = make_list_syn();
    }
    else{
        root -> list_syn = insert_at_beginning(root, root -> children -> next -> element -> list_syn, root -> children -> element);
        
    }
}

void driverModule(struct ParseTreeNode* root){
    copy_list_syn(root, root -> children -> next -> next -> next -> next -> element -> list_syn);
    
}

void ret(struct ParseTreeNode* root){
    if(!strcmp(root -> children -> element -> symbol, "RETURNS")){
        copy_list_syn(root, root -> children -> next -> next -> element -> list_syn);
    }
    else{
        root -> list_syn = NULL;
    }
}

void input_plist(struct ParseTreeNode* root){
    insert_addr(root -> children -> element, root -> children -> next -> next -> element);
    root -> list_syn = insert_at_beginning(root, root -> children -> next -> next -> next -> element -> list_syn, root -> children -> element);
    
}

void n1(struct ParseTreeNode* root){
    if(!strcmp(root -> children -> element -> symbol, "COMMA")){
        insert_addr(root -> children -> next -> element, root -> children -> next -> next -> next -> element);
        root -> list_syn = insert_at_beginning(root, root -> children -> next -> next -> next -> next -> element -> list_syn, root -> children -> next -> element);
        
    }
    else{
        root -> list_syn = make_list_syn();
        
    }
}

void output_plist(struct ParseTreeNode* root){

}

void n2(struct ParseTreeNode* root){
    
}

void dataType(struct ParseTreeNode* root){
    if(!strcmp(root -> children -> element -> symbol, "ARRAY")){
        root -> list_syn = make_list_syn();
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> next -> next -> next -> next -> element -> addr_syn);
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> next -> element);
    }
    else{
        insert_addr(root, root -> children -> element);
    }
    
}

void range_arrays(struct ParseTreeNode* root){
    root -> list_syn = make_list_syn();
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> next -> element);
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> element);
    
}

void type(struct ParseTreeNode* root){
    insert_addr(root, root -> children -> element);
}

void moduleDef(struct ParseTreeNode* root){

}

void statements(struct ParseTreeNode* root){
    if(!strcmp(root -> children -> element -> symbol, "#")){
        root -> list_syn = make_list_syn();
    }
    else{
        root -> list_syn = insert_at_beginning(root, root -> children -> next -> element -> list_syn, root -> children -> element);
    }
    
}

void statement(struct ParseTreeNode* root){
    copy_list_syn(root, root -> children -> element -> list_syn);
    
}

void ioStmt(struct ParseTreeNode* root){
    root -> list_syn = make_list_syn();
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> next -> element);
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> element);
}

void var_print(struct ParseTreeNode* root){
    if(!strcmp(root -> children -> element -> symbol, "ID")){
        root -> list_syn = make_list_syn();
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> element);
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> element);
    }
    else{
        insert_addr(root, root -> children -> element);
    }
}

void p1(struct ParseTreeNode* root){
    if(!strcmp(root -> children -> element -> symbol, "#")){
        
    }
    else{
        root -> list_syn = make_list_syn();
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> next -> element);
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> element);
    }
}

void boolConstt(struct ParseTreeNode* root){
    insert_addr(root, root -> children -> element);
}

void simpleStmt(struct ParseTreeNode* root){
    copy_list_syn(root, root -> children -> element -> list_syn);
}

void assignmentStmt(struct ParseTreeNode* root){
    copy_list_syn(root, root -> children -> next -> element -> list_syn);
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> element);
}

void whichStmt(struct ParseTreeNode* root){
    copy_list_syn(root, root -> children -> element -> list_syn);
}

void lvalueIDStmt(struct ParseTreeNode* root){
    root -> list_syn = make_list_syn();
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> element);
}

void lvalueARRStmt(struct ParseTreeNode* root){
    root -> list_syn = make_list_syn();
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> next -> next -> next -> element);
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> element);
}

void index_arr(struct ParseTreeNode* root){
    root -> list_syn = make_list_syn();
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> element);
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> element);
}

void new_index(struct ParseTreeNode* root){

}

void sign(struct ParseTreeNode* root){

}


void moduleReuseStmt(struct ParseTreeNode* root){
    root -> list_syn = make_list_syn();
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> next -> next -> next -> next -> next -> element);
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> next -> next -> element);
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> element);
    
}

void actual_para_list(struct ParseTreeNode* root){
    copy_list_syn(root, root -> children -> next -> next -> element -> list_syn);
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> element);
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> element);
}

void actual_para_list_prime(struct ParseTreeNode* root){
    if(root -> children -> next == NULL){
        root -> list_syn = make_list_syn();
    }
    else{
        copy_list_syn(root, root -> children -> next -> next -> next -> element -> list_syn);
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> next -> element);
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> element);
    }
}

void k(struct ParseTreeNode* root){
    if(root -> children -> next == NULL){
        insert_addr(root, root -> children -> element);
    }
    else{
        root -> list_syn = make_list_syn();
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> element);
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> element);
    }
}

void n_11(struct ParseTreeNode* root){
    if(root -> children -> next != NULL){
        insert_addr(root, root -> children -> next -> element);
    }
}

void optional(struct ParseTreeNode* root){
    if(strcmp(root -> children -> element -> symbol, "#")){
        copy_list_syn(root, root -> children -> next -> element -> list_syn);
    }
}

void declareStmt(struct ParseTreeNode* root){
    root -> list_syn = make_list_syn();
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> next -> next -> element);
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next  -> element);
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> element);
    
}

void idList(struct ParseTreeNode* root){
    copy_list_syn(root, root -> children -> next -> element -> list_syn);
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> element);
}

void n3(struct ParseTreeNode* root){
    if(!strcmp(root -> children -> element -> symbol, "COMMA")){
        root -> list_syn = insert_at_beginning(root, root -> children -> next -> next -> element -> list_syn, root -> children -> next -> element);
    }
    else{
        root -> list_syn = make_list_syn();
    }
}

void expression(struct ParseTreeNode* root){
    insert_addr(root, root -> children -> element);
}

void u(struct ParseTreeNode* root){
    root -> list_syn = make_list_syn();
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> element -> addr_syn);
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> element -> addr_syn);
}

void new_NT(struct ParseTreeNode* root){
    if(!strcmp(root -> children -> element -> symbol, "BO")){
        insert_addr(root, root -> children -> next -> element);
    }
    else{
        insert_addr(root, root -> children -> element -> addr_syn);
    }
}

void arithmeticOrBooleanExpr(struct ParseTreeNode* root){
    copy_list_syn(root, root -> children -> next -> element -> list_syn);
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> element);
    
}

void n7(struct ParseTreeNode* root){
    if(!strcmp(root -> children -> element -> symbol, "#")){
        root -> list_syn = make_list_syn();
    }
    else{
        copy_list_syn(root, root -> children -> next -> next -> element -> list_syn);
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> element);
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> element);
        
    }
}

void anyTerm(struct ParseTreeNode* root){
    if(root -> children -> next != NULL){
        insert_addr(root, root -> children -> next -> element -> addr_syn);
        copy_list_syn(root, root -> children -> next -> element -> list_syn);
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> element);
    }
    else{
        insert_addr(root, root -> children -> element);
    }
    
}

void n8(struct ParseTreeNode* root){
    if(root -> children -> next != NULL){
        insert_addr(root, root -> children -> element);
        root -> list_syn = make_list_syn();
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> element);
    }
    else{
        root -> list_syn = make_list_syn();
    }
}

void arithmeticExpr(struct ParseTreeNode* root){
    copy_list_syn(root, root -> children -> next -> element -> list_syn);
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> element);
}

void n4(struct ParseTreeNode* root){
    if(root -> children -> next == NULL){
        root -> list_syn = make_list_syn();
    }
    else{
        copy_list_syn(root, root -> children -> next -> next -> element -> list_syn);
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> element);
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> element -> addr_syn);
    }
}

void term(struct ParseTreeNode* root){

}

void factor(struct ParseTreeNode* root){
    if(root -> children -> next == NULL){
        insert_addr(root, root -> children -> element);
    }
    else if(root -> children -> next -> next == NULL){
        root -> list_syn = make_list_syn();
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> element);
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> element);
    }
    else{
        copy_list_syn(root, root -> children -> next -> element -> list_syn);
    }
}

void n5(struct ParseTreeNode* root){

}

void arrExpr(struct ParseTreeNode* root){

}

void arr_n4(struct ParseTreeNode* root){

}

void arrTerm(struct ParseTreeNode* root){

}

void arr_n5(struct ParseTreeNode* root){

}

void arrFactor(struct ParseTreeNode* root){
    if(root -> children -> next == NULL){
        insert_addr(root, root -> children -> element);
    }
    else{
        insert_addr(root, root -> children -> next -> element);
    }
}

void element_index_with_expressions(struct ParseTreeNode* root){
    if(root -> children -> next == NULL){
        copy_list_syn(root, root -> children -> element -> list_syn);
    }
    else{
        copy_list_syn(root, root -> children -> next -> element -> list_syn);
        insert_addr(root, root -> children -> element -> addr_syn);
    }
}

void n_10(struct ParseTreeNode* root){
    if(root -> children -> next == NULL){
        root -> list_syn = make_list_syn();
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> element -> addr_syn);
    }
    else{
        copy_list_syn(root, root -> children -> next -> element -> list_syn);
    }
}

void condionalStmt(struct ParseTreeNode* root){
    root -> list_syn = make_list_syn();
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> next -> next -> next -> next -> next -> element);
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> next -> next -> next -> next -> element);
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> next -> element);
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> element);
    
}

void caseStmts(struct ParseTreeNode* root){
    copy_list_syn(root, root -> children -> next -> next -> next -> next -> next -> next -> element -> list_syn);
}

void n9(struct ParseTreeNode* root){
    if(!strcmp(root -> children -> element -> symbol, "#")){
        root -> list_syn = make_list_syn();
    }
    else{
        root -> list_syn = insert_at_beginning(root, root -> children -> next -> next -> next -> next -> next -> next -> element -> list_syn, root);
    }
}

void default_(struct ParseTreeNode* root){
    if(!strcmp(root -> children -> element -> symbol, "DEFAULT")){
        insert_addr(root, root -> children -> next -> next -> element);
    }
}

void iterativeStmt(struct ParseTreeNode* root){
    if(!strcmp(root -> children -> element -> symbol, "FOR")){
        root -> list_syn = make_list_syn();
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> next -> next -> next -> next -> next -> next -> element);
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> next -> next -> next -> element);
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> next -> element);
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> element);
        
    }
    else{
        root -> list_syn = make_list_syn();
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> next -> next -> next -> next -> element);
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> next -> element);
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> element);
        
    }
}

void module(struct ParseTreeNode* root){
    root -> list_syn = make_list_syn();
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> next -> next -> next -> next -> next -> next -> next -> next -> next -> next -> element);
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> next -> next -> next -> next -> next -> next -> next -> next -> next -> element);
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> next -> next -> next -> next -> next -> next -> element);
    root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> next -> element);
}

void sign_for_loop(struct ParseTreeNode* root){
    if(!strcmp(root -> children -> element -> symbol, "MINUS")){
        insert_addr(root, root -> children -> element);
    }
    
}

struct ParseTreeNode* createAST(struct ParseTreeNode* root){
    //Post Order
    if(root == NULL){
        return NULL;
    }
    struct ParseTreeListNode* temp = root -> children;
    while(temp != NULL){
        createAST(temp -> element);
        temp = temp -> next;
    }

    //Ast creation case by case
    if(!strcmp("program", root -> symbol)){
        root -> list_syn = make_list_syn();
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> next -> next -> element);
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> next -> element);
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> element);
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> element);
        
    }
    else if(!strcmp("moduleDeclarations", root -> symbol)){
        moduleDeclarations(root);
    }
    else if(!strcmp("moduleDeclaration", root -> symbol)){
        moduleDeclaration(root);
    }
    else if(!strcmp("otherModules", root -> symbol)){
        otherModules(root);
    }
    else if(!strcmp("driverModule", root -> symbol)){
        driverModule(root);
    }
    else if(!strcmp("var_id_num", root -> symbol) || !strcmp("unary_op", root -> symbol) || !strcmp("op1", root -> symbol) 
    || !strcmp("op2", root -> symbol) || !strcmp("logicalOp", root -> symbol) || !strcmp("relationalOp", root -> symbol))
    {
        insert_addr(root, root -> children -> element);
    }
    else if(!strcmp("ret", root -> symbol)){
        ret(root);
    }
    else if(!strcmp("input_plist", root -> symbol) || !strcmp("output_plist", root -> symbol)){
        input_plist(root);
    }
    else if(!strcmp("n1", root -> symbol) || !strcmp("n2", root -> symbol)){
        n1(root);
    }
    else if(!strcmp("sign_for_loop", root -> symbol) || !strcmp("sign", root -> symbol)){
        sign_for_loop(root);
    }
    else if(!strcmp("range_for_loop", root -> symbol)){
        root -> list_syn = make_list_syn();
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> next -> element);
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> element);
    }
    else if(!strcmp("index_for_loop", root -> symbol)){
        root -> list_syn = make_list_syn();
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> next -> element);
        root -> list_syn = insert_at_beginning(root, root -> list_syn, root -> children -> element);
    }
    else if(!strcmp("new_index_for_loop", root -> symbol) || !strcmp("new_index", root -> symbol) || !strcmp("value", root -> symbol)){
        insert_addr(root, root -> children -> element);
    }
    else if(!strcmp("statements", root -> symbol)){
        statements(root);
    }
    else if(!strcmp("statement", root -> symbol)){
        statement(root);
    }
    else if(!strcmp("moduleDef", root -> symbol)){
        copy_list_syn(root, root -> children -> next -> element -> list_syn);
    }
    else if(!strcmp("type", root -> symbol)){
        type(root);
    }
    else if(!strcmp("boolConstt", root -> symbol)){
        boolConstt(root);
    }
    else if(!strcmp("simpleStmt", root -> symbol)){
        simpleStmt(root);
    }
    else if(!strcmp("p1", root -> symbol)){
        p1(root);
    }
    else if(!strcmp("var_print", root -> symbol)){
        var_print(root);
    }
    else if(!strcmp("ioStmt", root -> symbol)){
        ioStmt(root);
    }
    else if(!strcmp("default", root -> symbol)){
        default_(root);
    }
    else if(!strcmp("declareStmt", root -> symbol)){
        declareStmt(root);
    }
    else if(!strcmp("condionalStmt", root -> symbol)){
        condionalStmt(root);
    }
    else if(!strcmp("n9", root -> symbol)){
        n9(root);
    }
    else if(!strcmp("n3", root -> symbol)){
        n3(root);
    }
    else if(!strcmp("caseStmts", root -> symbol)){
        caseStmts(root);
    }
    else if(!strcmp("idList", root -> symbol)){
        idList(root);
    }
    else if(!strcmp("optional", root -> symbol)){
        optional(root);
    }
    else if(!strcmp("dataType", root -> symbol)){
        dataType(root);
    }
    else if(!strcmp("range_arrays", root -> symbol)){
        range_arrays(root);
    }
    else if(!strcmp("module", root -> symbol)){
        module(root);
    }
    else if(!strcmp("arithmeticOrBooleanExpr", root -> symbol)){
        arithmeticOrBooleanExpr(root);
    }
    else if(!strcmp("n7", root -> symbol)){
        n7(root);
    }
    else if(!strcmp("anyTerm", root -> symbol)){
        anyTerm(root);
    }
    else if(!strcmp("n8", root -> symbol)){
        n8(root);
    }
    else if(!strcmp("iterativeStmt", root -> symbol)){
        iterativeStmt(root);
    }
    else if(!strcmp("new_NT", root -> symbol)){
        new_NT(root);
    }
    else if(!strcmp("u", root -> symbol)){
        u(root);
    }
    else if(!strcmp("expression", root -> symbol)){
        expression(root);
    }
    else if(!strcmp("arrFactor", root -> symbol)){
        arrFactor(root);
    }
    else if(!strcmp("arithmeticExpr", root -> symbol) || !strcmp("term", root -> symbol) || !strcmp("arrExpr", root -> symbol)
    || !strcmp("arrTerm", root -> symbol)){
        arithmeticExpr(root);
    }
    else if(!strcmp("n4", root -> symbol) || !strcmp("n5", root -> symbol) || !strcmp("arr_n4", root -> symbol)
    || !strcmp("arr_n5", root -> symbol)){
        n4(root);
    }
    else if(!strcmp("element_index_with_expressions", root -> symbol)){
        element_index_with_expressions(root);
    }
    else if(!strcmp("n_10", root -> symbol)){
        n_10(root);
    }
    else if(!strcmp("n_11", root -> symbol)){
        n_11(root);
    }
    else if(!strcmp("k", root -> symbol)){
        k(root);
    }
    else if(!strcmp("factor", root -> symbol)){
        factor(root);
    }
    else if(!strcmp("moduleReuseStmt", root -> symbol)){
        moduleReuseStmt(root);
    }
    else if(!strcmp("actual_para_list", root -> symbol)){
        actual_para_list(root);
    }
    else if(!strcmp("actual_para_list'", root -> symbol)){
        actual_para_list_prime(root);
    }
    else if(!strcmp("assignmentStmt", root -> symbol)){
        assignmentStmt(root);
    }
    else if(!strcmp("whichStmt", root -> symbol)){
        whichStmt(root);
    }
    else if(!strcmp("lvalueIDStmt", root -> symbol)){
        lvalueIDStmt(root);
    }
    else if(!strcmp("lvalueARRStmt", root -> symbol)){
        lvalueARRStmt(root);
    }
    else if(!strcmp("index_arr", root -> symbol)){
        index_arr(root);
    }
    return root;
}