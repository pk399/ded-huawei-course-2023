#ifndef _CMD_H_
#define _CMD_H_

#include "instructions.h"

union {
	double farg;
	long int iarg;
	char inst[8];
} code_word;

enum PAR_T {
	NONE,
	LITERAL,
	REGISTER
};

code_word CWCtor(INSTRUCTIONS i, PAR_T p);

INSTRUCTIONS CWIns(code_word cw);
PAR_T CWPar(code_word cw);

#endif /* _CMD_H_ */
