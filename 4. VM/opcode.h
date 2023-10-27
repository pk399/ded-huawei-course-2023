#ifndef _BYTECODE_H_
#define _BYTECODE_H_

#include "instructions.h"

union code_word {
	double farg;
	long int iarg;
	char inst[8];
};

// struct opcode
// {
//     int code : 6;
//     int arg_type : 2;
// }

// typedef int64_t reg_type

const unsigned CW_SZ = sizeof(code_word);

enum PAR_T {
	NONE,
	LITERAL,
	REGISTER
};

code_word CWCtor(INSTRUCTIONS i, PAR_T p);

void CWDump(code_word);

INSTRUCTIONS CWIns(code_word cw);
PAR_T CWPar(code_word cw);

#endif /* _BYTECODE_H_ */
