#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"

Node* op_new(OPERAND op) {
    Node* n = tree_new();
    if (!n) return NULL;
    
    n->type = OP;
    n->op = op;
    
    return n;
}


Node* var_new(/* TODO: multiples var support */) {
   Node* n = tree_new();
   if (!n) return NULL;
   
   n->type = VAR;
   
   return n;
}


Node* lit_new(double lit) {
    Node* n = tree_new();
    if (!n) return NULL;
    
    n->type = LIT;
    n->lit = lit;
    
    return n;
}


Node* tree_new() {
    Node* n = (Node*) calloc(1, sizeof(Node));
    if (!n) return NULL;
    
    return n;
}


int _tree_check(const Node* n) {
    if (!n) return 0;
    
    if (n->left == n || n->right == n) return 1;
    if (n->right && n->right == n->left) return 1;
    
    // TODO: check the full tree
    
    return 0;
}


int tree_dump(const Node* n) {
    if (_tree_check(n)) return 1;
    
    if (!n) {
        printf("nil");
        return 0;
    }
    
    printf("(");
    if (tree_dump(n->left)) return 1;
    
    switch (n->type) {
        case OP:
            printf(" %s ", op2str(n->op));
            break;
        case VAR:
            printf(" x ");
            break;
        case LIT:
            printf(" %.2lf ", n->lit);
            break;
        default:
            printf(" :/ ");
    }
    
    if (tree_dump(n->right)) return 1;
    printf(")");
    
    return 0;
}


int tree_delete(Node* n) {
    if (_tree_check(n)) return 1;
    if (!n) return 0;
    
    if (tree_delete(n->left)) return 1;
    if (tree_delete(n->right)) return 1;
    free(n);
    
    return 0;
}


void _skip_whitespace(const char** c) {
    while (**c == ' '  ||
           **c == '\t' ||
           **c == '\n')
        (*c)++;
}


void _skip_token(const char** c) {
    while (**c != ')'  &&
           **c != ' '  &&
           **c != '\t' &&
           **c != '\n' &&
           **c != '\0')
        (*c)++;
}


void _next_token(const char** c, char* buf, unsigned sz) {
    const char* str = *c;
    
    unsigned i = 0;
    
    for ( ; 
         i < sz - 1 &&
         str[i] != ' ' &&
         str[i] != '\n' &&
         str[i] != '\t' &&
         str[i] != '\0';
         i++) {
        buf[i] = str[i];
    }
    
    buf[i] = '\0';
    
    *c += i;
}


Node* _tree_parse(const char** buf_ptr) {
    _skip_whitespace(buf_ptr);

    const char* buf = *buf_ptr;

    if (*buf != '(') {
        _skip_token(buf_ptr);
        return NULL; // TODO: nil check???
    }
    buf++;

    const unsigned BUF_SZ = 128;
    
    char token[BUF_SZ] = {};
    _next_token(&buf, token, BUF_SZ);
    
    Node* n = NULL;
    
    OPERAND op = str2op(token);
    if (op != VOO) {
        n = op_new(op);
    } else if (!strcmp(token, "x")) {
        n = var_new();
    } else {
        n = lit_new(atof(token));
    }
    
    n->left = _tree_parse(&buf);
    n->right = _tree_parse(&buf);

    buf++; // the (

    *buf_ptr = buf;
    return n;
}


Node* tree_parse(const char* buf) {
    return _tree_parse(&buf);
}


int tree_export(char*, unsigned sz, Node*) {
    return -1; // TODO
}
