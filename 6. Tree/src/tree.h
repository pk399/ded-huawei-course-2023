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

Node* tree_op(OPERAND op);
Node* tree_var(/* TODO: multiple var support */);
Node* tree_lit(double lit);

Node* tree_new();
Node* tree_copy(const Node*);
int tree_check(const Node* n);
int tree_dump(const Node*);
void node2str(char* buf, unsigned sz, const Node*);
int tree_delete(Node*);

Node* tree_parse(const char* buf);
int tree_export(char* buf, unsigned sz, const Node*);

#endif /* _TREE_H_ */
