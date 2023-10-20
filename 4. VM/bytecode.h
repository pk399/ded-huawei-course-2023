#ifndef _BYTECODE_H_
#define _BYTECODE_H_

#include "instructions.h"

union code_word {
	double farg;
	long int iarg;
	char inst[8];
};

enum PAR_T {
	NONE,
	LITERAL,
	REGISTER
};

code_word CWCtor(INSTRUCTIONS i, PAR_T p);

INSTRUCTIONS CWIns(code_word cw);
PAR_T CWPar(code_word cw);

#endif /* _BYTECODE_H_ */
