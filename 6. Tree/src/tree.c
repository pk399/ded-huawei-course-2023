#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"
#include "smiley.h"
#include "defines.h"

Node* tree_op(OPERAND op) {
    Node* n = tree_new();
    if (!n) return NULL;
    
    n->type = OP;
    n->op = op;
    
    return n;
}


Node* tree_var(/* TODO: multiple var support */) {
   Node* n = tree_new();
   if (!n) return NULL;
   
   n->type = VAR;
   
   return n;
}


Node* tree_lit(double lit) {
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


int tree_check(const Node* n) {
    if (!n) return 1;
    
    if (n->left == n || n->right == n) return 1;
    if (n->right && n->right == n->left) return 1;
    
    if (n->type == OP) {
        #define OP_DEF(id, name, prio, argc, ...) if (n->op == id && ((n->left || 0) + (n->right || 0) != argc)) {return 1;} else
        #include "op.h"
        #undef OP_DEF
        /* else */ ;
    } else {
        if (n->right || n->left) return 1;
    }
    
    // TODO: check the full tree
    
    return 0;
}


Node* tree_copy(const Node* n) {
    if (tree_check(n)) return NULL;
    
    Node* nn = tree_new();
    if (!nn) return NULL;
    
    memcpy(nn, n, sizeof(Node));
    
    nn->left = tree_copy(n->left);
    nn->right = tree_copy(n->right);
    
    return nn;
}


void node2str(char* buf, unsigned sz, const Node* n) {
    switch (n->type) {
        case OP:
            snprintf(buf, sz, "%s", op2str(n->op));
            break;
        case VAR:
            snprintf(buf, sz, "x");
            break;
        case LIT:
            snprintf(buf, sz, "%.0lf", n->lit);
            break;
        default:
            snprintf(buf, sz, SMILEY);
    }
}


int _tree_dump(const Node* n) {
    if (!n) {
        printf("nil");
        return 0;
    }

    if (tree_check(n)) return 1;
    
    printf("(");
    if (_tree_dump(n->left)) return 1;
    
    char buf[BUF_SZ] = {};
    node2str(buf, BUF_SZ, n);
    printf(" %s ", buf);
    
    if (_tree_dump(n->right)) return 1;
    printf(")");
    
    return 0;
}


int tree_dump(const Node* n) {
    int res = _tree_dump(n);
    putchar('\n');
    
    return res;
}


int tree_delete(Node* n) {
    if (tree_check(n)) return 1;
    if (!n) return 0;
    
    if (tree_delete(n->left)) return 1;
    if (tree_delete(n->right)) return 1;
    free(n);
    
    return 0;
}


void _skip_whitespace(const char** c) {
    while (**c == ' '  ||
           **c == '\t' ||
           **c == '\n') // TODO isspace()
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
    
    char token[BUF_SZ] = {};
    _next_token(&buf, token, BUF_SZ);
    
    Node* n = NULL;
    
    OPERAND op = str2op(token);
    if (op != VOO) {
        n = tree_op(op);
    } else if (!strcmp(token, "x")) {
        n = tree_var();
    } else {
        n = tree_lit(atof(token));
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


int _tree_export(char** buf, unsigned* sz, const Node* n) {
    if (!n) {
        SHPRINTF(buf, sz, "nil");
        return 0;
    }    
    
    if (tree_check(n)) return -1;
    
    char buf2[BUF_SZ] = {};
    node2str(buf2, BUF_SZ, n);
    SHPRINTF(buf, sz, "(%s ", buf2);
    
    _tree_export(buf, sz, n->left);
    SHPRINTF(buf, sz, " ");
    _tree_export(buf, sz, n->right);
    
    SHPRINTF(buf, sz, ")");
    
    return 0; // TODO: return is broken
}


int tree_export(char* buf, unsigned sz, const Node* n) {
    return _tree_export(&buf, &sz, n);
}