#include <string.h>
//#include <stdio.h>

#include "lang.h"

const char* op2str(OPERAND op) {
    switch (op) {
        case ADD:  return "add";
        case SUB:  return "sub";
        case MUL:  return "mul";
        case DIV:  return "div";
        case SQRT: return "sqrt";
        case SIN:  return "sin";
        case COS:  return "cos";
        case PI:   return "pi";
        default:   return ":/";
    }
}


OPERAND str2op(const char* c) {
    if (!strcmp(c, "add")) return ADD;
    if (!strcmp(c, "sub")) return SUB;
    if (!strcmp(c, "mul")) return MUL;
    if (!strcmp(c, "div")) return DIV;
    if (!strcmp(c, "sqrt")) return SQRT;
    if (!strcmp(c, "sin")) return SIN;
    if (!strcmp(c, "cos")) return COS;
    if (!strcmp(c, "pi")) return PI;
    
    return VOO;
}


unsigned op_prio(OPERAND op) {
    switch (op) {
        case ADD: return 3;
        case SUB: return 2;
        case MUL: return 4;
        case DIV:
        case SQRT:
        case SIN:
        case COS:
            return 1;
        case PI:
        default:
            return NULL_PRIO;
    }
}
