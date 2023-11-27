#ifndef _LANG_H_
#define _LANG_H_

const unsigned NULL_PRIO = 0;

// To add a new op:
// 1. enum
// 2. op2str & str2op
// 3. op_prio
// 4. node2str
// 5. tree_latex
// 6. eval
// 7. d
// 8. _tree_check

enum OPERAND {
    #define OP_DEF(id, name, ...) OP_ ## name = id,
    #include "op.h"
    #undef OP_DEF

    VOO
};

const char* op2str(OPERAND);
OPERAND str2op(const char*);

unsigned op_prio(OPERAND);

#endif /* _LANG_H_ */
