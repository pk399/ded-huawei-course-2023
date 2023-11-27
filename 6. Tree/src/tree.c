#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "tree.h"

const unsigned BUF_SZ = 10000;//128; // Default size for temp buffers

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


int _tree_check(const Node* n) {
    if (!n) return 1;
    
    if (n->left == n || n->right == n) return 1;
    if (n->right && n->right == n->left) return 1;
    
    if (n->type == OP) {
        switch (n->op) {
            case PI:
            case E:
                break;
            case SQRT:
            case SIN:
            case COS:
            case LN:
                if (n->right) return 1;
                if (!n->left) return 1;
                break;
            default:
                if (!(n->right && n->left)) return 1;
        }
    } else {
        if (n->right || n->left) return 1;
    }
    
    // TODO: check the full tree
    
    return 0;
}


Node* tree_copy(const Node* n) {
    if (_tree_check(n)) return NULL;
    
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
            snprintf(buf, sz, "%.2lf", n->lit);
            break;
        default:
            snprintf(buf, sz, ":/");
    }
}


int _tree_dump(const Node* n) {
    if (!n) {
        printf("nil");
        return 0;
    }

    if (_tree_check(n)) return 1;
    
    printf("(");
    if (_tree_dump(n->left)) return 1;
    
    putchar(' ');
    char buf[BUF_SZ] = {};
    node2str(buf, BUF_SZ, n);
    puts(buf);
    putchar(' ');
    
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


#define SHPRINTF(buf, sz, ...) if (*(sz) != 0) {int tmp = snprintf(*(buf), *(sz), __VA_ARGS__); *(buf) += tmp; if (tmp < *(sz) - 1) {*(sz) -= tmp;} else {*(sz) = 0;}}


int _tree_export(char** buf, unsigned* sz, const Node* n) {
    if (!n) {
        SHPRINTF(buf, sz, "nil");
        return 0;
    }    
    
    if (_tree_check(n)) return -1;
    
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


#define L(n) (n)->left
#define R(n) (n)->right

#define LL(op) _tree_latex(buf, sz, L(n), op_prio(op));
#define LR(op) _tree_latex(buf, sz, R(n), op_prio(op));
#define BRACE(op, br) if (prev_prio > op_prio(op) || (prev_prio == op_prio(op) && (op) == DIV)) {SHPRINTF(buf, sz, (br))}
#define LBRACE(op) BRACE(op, "(")
#define RBRACE(op) BRACE(op, ")")

#define LEFT(op) LBRACE(op); LL(op)
#define RIGHT(op) LR(op); RBRACE(op)


int _tree_latex(char** buf, unsigned* sz, const Node* n, unsigned prev_prio) {
    if (!n) {
        SHPRINTF(buf, sz, "Naf");
        return 1;
    }    
    
    if (_tree_check(n)) return 1;
    
    switch (n->type) {
        //
        case LIT:
            SHPRINTF(buf, sz, "%.2lf", n->lit);
            break;
        case VAR:
            SHPRINTF(buf, sz, "x");
            break;
        case OP:
            switch (n->op) {
                case ADD:
                    LEFT(ADD);
                    SHPRINTF(buf, sz, " + ");
                    RIGHT(ADD);
                    break;
                case SUB:
                    LEFT(SUB);
                    SHPRINTF(buf, sz, " - ");
                    RIGHT(SUB);
                    break;
                case MUL:
                    LEFT(MUL);
                    SHPRINTF(buf, sz, " * ");
                    RIGHT(MUL);
                    break;
                case DIV:
                    LBRACE(DIV);
                    SHPRINTF(buf, sz, "\\frac{");
                    LL(DIV);
                    SHPRINTF(buf, sz, "}{");
                    LR(DIV);
                    SHPRINTF(buf, sz, "}");
                    RBRACE(DIV);
                    break;
                case SQRT:
                    SHPRINTF(buf, sz, "\\sqrt{");
                    LL(SQRT);
                    SHPRINTF(buf, sz, "}");
                    break;
                case SIN:
                    SHPRINTF(buf, sz, "sin(");
                    LL(SIN);
                    SHPRINTF(buf, sz, ")");
                    break;
                case COS:
                    SHPRINTF(buf, sz, "cos(");
                    LL(COS);
                    SHPRINTF(buf, sz, ")");
                    break;
                case PI:
                    SHPRINTF(buf, sz, "\\pi");
                    break;
                case POW:
                    LEFT(POW);
                    SHPRINTF(buf, sz, "^{");
                    LR(POW);
                    SHPRINTF(buf, sz, "}");
                    RBRACE(POW);
                    break;
                case LOG:
                    LBRACE(LOG);
                    SHPRINTF(buf, sz, "\\log_{");
                    LL(LOG);
                    SHPRINTF(buf, sz, "} ");
                    RIGHT(LOG);
                    break;
                case LN:
                    SHPRINTF(buf, sz, "\\ln ");
                    LL(SQRT);
                    break;
                case E:
                    SHPRINTF(buf, sz, "e");
                    break;
                default:
                    SHPRINTF(buf, sz, ":/ (op)");
            }
            break;
        default:
            SHPRINTF(buf, sz, ":/");
    }
    
    return 0;
}


int tree_latex(char* buf, unsigned sz, const Node* n) {
    SHPRINTF(&buf, &sz, "\\documentclass[12pt, a4paper]{article}\n\\begin{document}\n$$ ");
    int res = _tree_latex(&buf, &sz, n, NULL_PRIO);
    SHPRINTF(&buf, &sz, " $$ \n\\end{document}");
    
    return res;
}


void latex2pdf(const char* filename, const char* buf) { // TODO: Move into separate file?
    char buf2[BUF_SZ] = {};
    
    snprintf(buf2, BUF_SZ, "%s.tex", filename);
    FILE* f = fopen(buf2, "wt");
    if (!f) return; // TODO: -1
    fwrite(buf, 1, strlen(buf), f);
    fclose(f);

    snprintf(buf2, BUF_SZ, "pdflatex %s.tex", filename);
    f = popen(buf2, "r");
    if (!f) return;
    // Read, so the process doesn't block
    fread(buf2, 1, BUF_SZ, f);
    if (pclose(f)) return;
    
    snprintf(buf2, BUF_SZ, "rm %s.tex %s.log %s.aux", filename, filename, filename);
    f = popen(buf2, "r");
    if (!f) return;
    if (pclose(f)) return;
}


#define EL(n) eval(L(n), x)
#define ER(n) eval(R(n), x)


double eval(const Node* n, double x) {
    if (_tree_check(n)) return NAN;
    
    switch (n->type) {
        case LIT:
            return n->lit;
        case VAR:
            return x;
        case OP:
            switch (n->op) {
                case ADD:
                    return EL(n) + ER(n);
                case SUB:
                    return EL(n) - ER(n);
                case MUL:
                    return EL(n) * ER(n);
                case DIV:
                    return EL(n) / ER(n);
                case SQRT:
                    return sqrt(EL(n));
                case SIN:
                    return sin(EL(n));
                case COS:
                    return cos(EL(n));
                case PI:
                    return M_PI;
                case POW:
                    return pow(EL(n), ER(n));
                case LOG:
                    return log(ER(n)) / log(EL(n));
                case LN:
                    return log(EL(n));
                case E:
                    return 2.7182818284590452; // TODO
            }
    }
    
    return NAN;
}


int _is_constant(const Node* n) {
    if (!n) return 1;
    
    if (_tree_check(n)) return -1337;
    
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
#define _MUL(left, right) _OP(MUL, (left), (right))
#define _ADD(left, right) _OP(ADD, (left), (right))
#define _SUB(left, right) _OP(SUB, (left), (right))
#define _DIV(left, right) _OP(DIV, (left), (right))
#define _POW(left, right) _OP(POW, (left), (right))
#define _LN(left) _OP1(LN, (left))
#define _SQRT(left) _OP1(SQRT, (left))
#define _SIN(left) _OP1(SIN, (left))
#define _COS(left) _OP1(COS, (left))

#define C(n) tree_copy(n)


Node* d(const Node* n) {
    if (_tree_check(n)) return NULL;
    
    switch (n->type) {
        case LIT:
            return tree_lit(0);
        case VAR:
            return tree_lit(1);
        case OP:
            switch (n->op) {
                case ADD:
                    return _ADD(d(L(n)), d(R(n)));
                case SUB:
                    return _SUB(d(L(n)), d(R(n)));
                case MUL:
                    return _ADD(_MUL(C(L(n)), d(R(n))), _MUL(d(L(n)), C(R(n))));
                case DIV:
                    return _DIV(_SUB(_MUL(d(L(n)), C(R(n))), _MUL(C(L(n)), d(R(n)))), _POW(C(R(n)), tree_lit(2)));
                case SQRT:
                    return _MUL(_DIV(tree_lit(1), _MUL(tree_lit(2), _SQRT(C(L(n))))), d(L(n)));
                case SIN:
                    return _MUL(_COS(C(L(n))), d(L(n)));
                case COS:
                    return _MUL(_SUB(tree_lit(0), _SIN(C(L(n)))), d(L(n)));
                case PI:
                    return tree_lit(0);
                case POW:
                    if (_is_constant(L(n))) {
                        return _MUL(_MUL(C(n), _LN(C(L(n)))), d(R(n)));
                    } else if (_is_constant(R(n))) {
                        return _MUL(_MUL(C(R(n)), _POW(C(L(n)), _SUB(C(R(n)), tree_lit(1)))), d(L(n)));
                    } else {
                        return _MUL(_POW(C(L(n)), _SUB(C(R(n)), tree_lit(1))), _ADD(_MUL(C(R(n)), d(L(n))), _MUL(_MUL(C(L(n)), _LN(C(L(n)))), d(R(n)))));
                    }
                case E:
                    return tree_lit(0);
                case LOG:
                    if (_is_constant(L(n))) {
                        return _MUL(_DIV(tree_lit(1), _MUL(_LN(C(L(n))), C(R(n)))), d(R(n)));
                    } else {
                        return _DIV(_SUB(_DIV(_MUL(_LN(C(L(n))), d(R(n))), C(R(n))), _DIV(_MUL(d(L(n)), _LN(R(n))), C(L(n)))), _POW(_LN(C(L(n))), tree_lit(2)));
                    }
                case LN:
                    return _MUL(_DIV(tree_lit(1), C(L(n))), d(L(n)));
            }
    }
    
    return NULL;
}
