#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <stdint.h>

#include "../include/lexer.h"

struct ASTNode {
    enum TokenType op;          
    int precedence;                   
    struct ASTNode *left;                
    struct ASTNode *right;
    struct ASTNode *parent;
    
    union 
    {
       int int_val;
       int var_id;
    } value;
};

struct Parser;

extern struct ASTNode* create_ast_node(enum TokenType op, struct ASTNode* left, struct ASTNode* right);

extern void destroy_ast_node(struct ASTNode* root);

extern struct ASTNode* create_leaf_node(enum TokenType op);

extern void log_tree(struct ASTNode* root);

extern void make_ast_from_expr(struct ASTNode** root, struct Parser* parser);

extern struct ASTNode* run_ast_tree(struct ASTNode* root);

#endif //!EXPRESSION_H