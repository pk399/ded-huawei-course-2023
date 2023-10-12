#include <stdio.h>

#include "cmd.h"


char CmdCtor(enum INSTRUCTIONS i) {
	return i & 0x1f;
}


char CmdCtorPar(enum INSTRUCTIONS i, enum PARAM_TYPE pt) {
	char res = CmdCtor(i);

	switch (pt)	{
		case LITERAL:
			res |= 1 << 5;
			break;
		case REGISTER:
			res |= 1 << 6;
			break;
		default:
			printf("CmdCtorPar: reached default case\n");
	}
	
	return res;
}


enum INSTRUCTIONS CmdI(char cmd) {
	return (enum INSTRUCTIONS) (cmd & 0x1f);
}


enum PARAM_TYPE CmdPar(char cmd) {
	switch (cmd & 0xe0) {
		case (1 << 5):
			return LITERAL;
		case (1 << 6):
			return REGISTER;
		default:
			return (enum PARAM_TYPE) -printf("CmdPar: reached default case\n");
	}
}
