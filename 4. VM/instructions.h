#ifndef _INSTRUCTIONS_H_
#define _INSTRUCTIONS_H_

enum INSTRUCTIONS {
	HLT = 0,
	PUSH,
	POP,
	IN,
	OUT,
	ADD,
	SUB,
	MUL,
	DIV,
	SQRT,
	SIN,
	COS
};


const char INAMES[][5] = {"hlt",
						  "push",
					 	  "pop",
						  "in",
						  "out",
					 	  "add",
						  "sub",
						  "mul",
						  "div",
						  "sqrt",
						  "sin",
						  "cos"};

#endif /* _INSTRUCTIONS_H_ */
