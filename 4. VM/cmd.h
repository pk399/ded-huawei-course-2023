#ifndef _CMD_H_
#define _CMD_H_

#include "instructions.h"

enum PARAM_TYPE {
	LITERAL,
	REGISTER
};

char CmdCtor(enum INSTRUCTIONS i);
char CmdCtorPar(enum INSTRUCTIONS i, enum PARAM_TYPE param_type);

enum INSTRUCTIONS CmdI(char cmd);
enum PARAM_TYPE CmdPar(char cmd);

#endif /* _CMD_H_ */
