#ifndef _OPCODE_H_
#define _OPCODE_H_

enum ARG_TYPE {
	NOP,
	IMM,
	REG
};

// Format:
// 0  1  0  1  1  1  1  1
//   REG   \------------|
//      IMM     CMD

char OPCtor(unsigned num, ARG_TYPE argt);

ARG_TYPE OPGetArg(char opcode);
unsigned OPGetCmd(char opcode);

#endif /* _OPCODE_H_ */
