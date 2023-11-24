#ifndef _LANG_H_
#define _LANG_H_

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

#endif /* _LANG_H_ */
