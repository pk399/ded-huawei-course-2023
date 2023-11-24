#ifndef _TREE_H_
#define _TREE_H_

#include "lang.h"

enum TOKEN {
    LIT,
    VAR,
    OP
};


struct Node {
    TOKEN type;
    
    union {
        double lit;
        unsigned var;
        OPERAND op;
    };
    
    Node* left;
    Node* right;
};

Node* op_new(OPERAND op);
Node* var_new(/* TODO: multiples var support */);
Node* lit_new(double lit);

Node* tree_new();
int tree_dump(const Node*);
int tree_delete(Node*);

Node* tree_parse(const char* buf);
int tree_export(char*, unsigned sz, Node*);

#endif /* _TREE_H_ */
