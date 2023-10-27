#include "cmd.h"

#include "opcode.h"


ARG_TYPE OPGetArg(char opcode) {
    switch (opcode & 0x60) {
        case (1 << 5):
            return IMM;
        case (1 << 6):
            return REG;
        default:
            return NOP;
    }
}


unsigned OPGetCmd(char opcode) {
    unsigned num = opcode & 0x1f;

    #define DEF_CMD(num, name, ...) if (opcode == num) return num; else
    
    #include "cmd_def.h"
    
    #undef DEF_CMD
    /* else */ if (0) ;
    
    return 0; // HLT
}
