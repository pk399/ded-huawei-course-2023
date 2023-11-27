#include <string.h>
//#include <stdio.h>

#include "lang.h"
#include "smiley.h"

const char* op2str(OPERAND op) {
    switch (op) {
        #define OP_DEF(id, name, ...) case id: return #name;
        #include "op.h"
        #undef OP_DEF

        default:
            return SMILEY;
    }
}


OPERAND str2op(const char* c) {
    #define OP_DEF(id, name, ...) if (!strcmp(c, #name)) return (OPERAND) id;
    #include "op.h"
    #undef OP_DEF
    
    return VOO;
}


unsigned op_prio(OPERAND op) {
    switch (op) {
        #define OP_DEF(id, name, prio, ...) case id: return prio;
        #include "op.h"
        #undef OP_DEF
        
        default:
            return NULL_PRIO;
    }
}
