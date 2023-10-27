#include "opcode.h"


char OPCtor(unsigned num, ARG_TYPE argt) {
    char opcode = num & 0x1f;
    
    switch (argt) {
        case IMM:
            opcode = opcode | (1 << 5);
            break;
        case REG:
            opcode = opcode | (1 << 6);
            break;
    } 
    return opcode;
}


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

    #define DEF_CMD(NUM, NAME, ...) if (num == NUM) return NUM; else
    
    #include "cmd_def.h"
    
    #undef DEF_CMD
    /* else */ if (0) ;
 
    return 0; // HLT
}
