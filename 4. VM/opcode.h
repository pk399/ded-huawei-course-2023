#ifndef _OPCODE_H_
#define _OPCODE_H_

struct Arg_t {
	int imm;
	int reg;
	int mem;
};

// Format:
// 0  1  0  1  1  1  1  1
//   REG   \------------|
//MEM   IMM     CMD

char OPCtor(unsigned num, Arg_t argt);

Arg_t OPGetArg(char opcode);
unsigned OPGetCmd(char opcode);

#endif /* _OPCODE_H_ */
