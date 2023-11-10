#include "opcode.h"


char OPCtor(unsigned num, Arg_t argt) {
    char opcode = num & 0x1f;
    
    if (argt.imm) opcode |= (1 << 5);
    if (argt.reg) opcode |= (1 << 6);
    if (argt.mem) opcode |= (1 << 7);
    
    return opcode;
}


Arg_t OPGetArg(char opcode) {
    Arg_t res = {((opcode & (1 << 5)) != 0),
                 ((opcode & (1 << 6)) != 0),
                 ((opcode & (1 << 7)) != 0)};

    return res;
}


unsigned OPGetCmd(char opcode) {
    unsigned num = opcode & 0x1f;

    #define DEF_CMD(NUM, NAME, ...) if (num == NUM) return NUM; else
    
    #include "cmd_def.h"
    
    #undef DEF_CMD
    /* else */ if (0) ;
 
    return 0; // HLT
}
