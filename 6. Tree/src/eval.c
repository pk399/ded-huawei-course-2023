#include <math.h>

#include "eval.h"

#include "tree.h"
#include "defines.h"

#define EL eval(L(n), x)
#define ER eval(R(n), x)


double eval(const Node* n, double x) {
    if (tree_check(n)) return NAN;
    
    switch (n->type) {
        case LIT:
            return n->lit;
        case VAR:
            return x;
        case OP:
            switch (n->op) {
                #define OP_DEF(id, name, prio, argc, eval) case id: return (eval);
                #include "op.h"
                #undef OP_DEF
            }
    }
    
    return NAN;
}