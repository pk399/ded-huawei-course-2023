#include <stdio.h>


#include "bytecode.h"


code_word CWCtor(INSTRUCTIONS i, PAR_T p) {
	code_word cw = {0};
	
	cw.inst[0] = i;

	switch (p) {
		case LITERAL:
			cw.inst[1] = 1;
			break;
		case REGISTER:
			cw.inst[2] = 1;
			break;
		case NONE:
		default:
			break;
	}
	
	return cw;
}


void CWDump(code_word cw) {
	printf("%04ld ", cw.iarg);
}


INSTRUCTIONS CWIns(code_word cw) {
	return (INSTRUCTIONS) cw.inst[0];
}


PAR_T CWPar(code_word cw) {
	if ( cw.inst[1] == 1 )
		return LITERAL;
	else if ( cw.inst[2] == 1)
		return REGISTER;
	
	return NONE;
}
