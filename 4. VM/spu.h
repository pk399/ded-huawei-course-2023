#ifndef _SPU_H_
#define _SPU_H_

#include <stdio.h>


const unsigned DEFAULT_CODE_SIZE = 512;
const unsigned MEMORY_SIZE = 10; // One side of a square
const unsigned DUMP_LIMIT = 16;
const unsigned MX_CMD_BUF = 8; // Max to_push[], poped[] size


struct SPU {
	Stack* stack;
	
	double regs[REG_COUNT];
	
	unsigned ip;
	char* code;
	unsigned code_sz;
	
	Stack* call_stk;
	
	double memory[MEMORY_SIZE * MEMORY_SIZE]; // memory.h legacy
};


enum STEP_RES {
    HALTED,
    NOT_HALTED
};


SPU* SPUCtor();
int SPUDump(SPU*);
int SPUDtor(SPU*);

int SPULoad(SPU*, const char* filename);
int fSPULoad(SPU*, FILE* file);

STEP_RES SPUStep(SPU*);
int SPUExec(SPU*);

#endif /* _SPU_H_ */
