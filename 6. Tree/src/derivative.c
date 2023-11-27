#include "derivative.h"

#include "lang.h"
#include "tree.h"
#include "defines.h"

int _is_constant(const Node* n) {
    if (!n) return 1;
    
    if (tree_check(n)) return -1337;
    
    int res = 0;
        
    if (n->type == LIT)
        res = 1;
    
    // TODO: check for errors
    return res * _is_constant(L(n)) * _is_constant(R(n));
}


Node* _tree_op_lr(OPERAND op, Node* left, Node* right) {
    Node* n = tree_op(op);
    if (!n) return NULL;
    
    n->left = left;
    n->right = right;
    
    return n;
}


Node* _tree_op_l(OPERAND op, Node* left) {
    Node* n = tree_op(op);
    if (!n) return NULL;
    
    n->left = left;
    
    return n;
}


// A little DSL
#define _OP(op, left, right) _tree_op_lr((op), (left), (right))
#define _OP1(op, left) _tree_op_l((op), (left))
#define _MUL(left, right) _OP(OP_MUL, (left), (right))
#define _ADD(left, right) _OP(OP_ADD, (left), (right))
#define _SUB(left, right) _OP(OP_SUB, (left), (right))
#define _DIV(left, right) _OP(OP_DIV, (left), (right))
#define _POW(left, right) _OP(OP_POW, (left), (right))
#define _LN(left) _OP1(OP_LN, (left))
#define _SQRT(left) _OP1(OP_SQRT, (left))
#define _SIN(left) _OP1(OP_SIN, (left))
#define _COS(left) _OP1(OP_COS, (left))

#define C(n) tree_copy(n)


Node* derivative(const Node* n) {
    if (tree_check(n)) return NULL;
    
    switch (n->type) {
        case LIT:
            return tree_lit(0);
        case VAR:
            return tree_lit(1);
        case OP:
            switch (n->op) {
                case OP_ADD:
                    return _ADD(derivative(L(n)), derivative(R(n)));
                case OP_SUB:
                    return _SUB(derivative(L(n)), derivative(R(n)));
                case OP_MUL:
                    return _ADD(_MUL(C(L(n)), derivative(R(n))), _MUL(derivative(L(n)), C(R(n))));
                case OP_DIV:
                    return _DIV(_SUB(_MUL(derivative(L(n)), C(R(n))), _MUL(C(L(n)), derivative(R(n)))), _POW(C(R(n)), tree_lit(2)));
                case OP_SQRT:
                    return _MUL(_DIV(tree_lit(1), _MUL(tree_lit(2), _SQRT(C(L(n))))), derivative(L(n)));
                case OP_SIN:
                    return _MUL(_COS(C(L(n))), derivative(L(n)));
                case OP_COS:
                    return _MUL(_SUB(tree_lit(0), _SIN(C(L(n)))), derivative(L(n)));
                case OP_PI:
                    return tree_lit(0);
                case OP_POW:
                    if (_is_constant(L(n))) {
                        return _MUL(_MUL(C(n), _LN(C(L(n)))), derivative(R(n)));
                    } else if (_is_constant(R(n))) {
                        return _MUL(_MUL(C(R(n)), _POW(C(L(n)), _SUB(C(R(n)), tree_lit(1)))), derivative(L(n)));
                    } else {
                        return _MUL(_POW(C(L(n)), _SUB(C(R(n)), tree_lit(1))), _ADD(_MUL(C(R(n)), derivative(L(n))), _MUL(_MUL(C(L(n)), _LN(C(L(n)))), derivative(R(n)))));
                    }
                case OP_E:
                    return tree_lit(0);
                case OP_LOG:
                    if (_is_constant(L(n))) {
                        return _MUL(_DIV(tree_lit(1), _MUL(_LN(C(L(n))), C(R(n)))), derivative(R(n)));
                    } else {
                        return _DIV(_SUB(_DIV(_MUL(_LN(C(L(n))), derivative(R(n))), C(R(n))), _DIV(_MUL(derivative(L(n)), _LN(R(n))), C(L(n)))), _POW(_LN(C(L(n))), tree_lit(2)));
                    }
                case OP_LN:
                    return _MUL(_DIV(tree_lit(1), C(L(n))), derivative(L(n)));
            }
    }
    
    return NULL;
}