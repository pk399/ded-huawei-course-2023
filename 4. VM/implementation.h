#ifndef _IMPLEMENTATION_H_
#define _IMPLEMENTATION_H_

#include "func_stack.h"
#include "instructions.h"
#include "vm.h"

typedef void (*command)(Stack*, char* registers, char parameter);

void pop(Stack* stk, char* registers, char parameter);

typedef struct _cmd {
	INSTRUCTIONS instruction;
	command function;
} Command;

const Command abc[] = {{POP, pop}};


#endif /* _IMPLEMENTATION_H_ */
