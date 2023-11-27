#ifndef _LANG_H_
#define _LANG_H_

const unsigned NULL_PRIO = 0;

enum OPERAND {
    VOO,
    ADD,
    SUB,
    MUL,
    DIV,
    SQRT,
    SIN,
    COS,
    PI
};

const char* op2str(OPERAND);
OPERAND str2op(const char*);

unsigned op_prio(OPERAND);

#endif /* _LANG_H_ */
