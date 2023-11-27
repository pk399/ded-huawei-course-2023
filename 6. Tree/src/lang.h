#ifndef _LANG_H_
#define _LANG_H_

const unsigned NULL_PRIO = 0;

// To add a new op:
// 1. enum
// 2. op2str & str2op
// 3. op_prio
// 4. tree_latex

enum OPERAND {
    VOO,
    ADD,
    SUB,
    MUL,
    DIV,
    SQRT,
    SIN,
    COS,
    PI,
    POW
};

const char* op2str(OPERAND);
OPERAND str2op(const char*);

unsigned op_prio(OPERAND);

#endif /* _LANG_H_ */
