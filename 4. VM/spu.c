#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "stack.h"
#include "colors.h"
#include "memory.h"
#include "bytecode.h"
#include "instructions.h"
#include "bcfile.h"
#include "error.h"

#include "spu.h"

const unsigned REG_COUNT = 4;

struct _spu {
	Stack* stack;
	code_word registers[REG_COUNT];
	Memory* code;
};


SPU* SPUCtor() {
	SPU* spu = (SPU*) calloc(1, sizeof(SPU));
	if (!spu) return spu;
	
	spu->stack = StackCtor(CW_SZ);
	spu->code = MemCtor(CW_SZ);
	if (!spu->stack || !spu->code) {
		free(spu);
		StackDtor(spu->stack);
		MemDtor(spu->code);
		return NULL;
	}
	
	return spu;
}


int SPUDtor(SPU* spu) {
	if (spu) {
		StackDtor(spu->stack);
		MemDtor(spu->code);
		free(spu);
	}
	
	return 0;
}


int SPUDump(SPU* spu) {
	printf(GREEN("SPU") "[" BLUE("%p") "] {\n", spu);
	
	printf("\t");
	StackDump(spu->stack);
	
	printf("\t" GREEN("Registers "));
	for (unsigned i = 0; i < REG_COUNT; i++)
		CWDump(spu->registers[i]);
	printf("\n");
	
	printf("\t");
	MemDump(spu->code);
	
	printf("}\n");
	
	return 0;
}


int SPULoad(SPU* spu, const char* filename) {
	FILE* f = fopen(filename, "rb");
	if (!f) {
		FATAL("SPULoad: Error opening file");
		return -1;
	}
	
	int res = fSPULoad(spu, f);
	
	fclose(f);
	
	return res;
}


int fSPULoad(SPU* spu, FILE* file) {
	assert(spu);
	assert(file);

	return ReadBytecodeMem(spu->code, file);
}


int SPUStackPush(SPU* spu, code_word val) {
	return StackPush(spu->stack, &val);
}


int SPUStackPop(SPU* spu, code_word* val) {
	return StackPop(spu->stack, val);
}


int SPUStep(SPU* spu) {
	code_word cmd = {0};
	RELAY( MemRead(spu->code, &cmd) );
	MemShift(spu->code, 1);
	
	PAR_T pt = CWPar(cmd);
	
	code_word arg = {0};
	if (pt) {
		RELAY( MemRead(spu->code, &arg) );
		MemShift(spu->code, 1);
		
	}
	
	if (pt == REGISTER &&
		( arg.iarg < 0 || arg.iarg >= REG_COUNT))
		return -1;
	
	switch ( CWIns(cmd) ) {
		#define HALT return 1;
		#define ARG arg
		#define ARG_T code_word
		#define ARG_ZERO {0}
		#define PT pt
		#define A_REG REGISTER
		#define A_LIT LITERAL
		#define ERR(t) FATAL(t)
		#define WARN(t) WARNING(t)
		#define CANT return -1
		#define DO_PUSH(val) SPUStackPush(spu, (val))
		#define DO_POP(val) SPUStackPop(spu, &(val))
		#define REG(arg) spu->registers[arg.iarg]
		#define I(arg) (arg).iarg
		#define F(arg) (arg).farg
		#define IARG(arg) (code_word) { .iarg = (arg) }
		#define FARG(arg) (code_word) { .farg = (arg) }
		#define DC(num, name, has_arg, ...) case CMD_ ## name:  \
												{               \
													__VA_ARGS__ \
												} break;
		#include "commands.h"
		default:
			FATAL("SPU reached default case!");
			return -1;
	}
	
	return 0;
}


int SPUExec(SPU* spu) {
	while ( !SPUStep(spu) )
		/*SPUDump(spu)*/;
		
	return 0;
}


int main() {
	SPU* spu = SPUCtor();
	
	//SPUDump(spu);
	
	printf("Reading bytecode...\n");
	SPULoad(spu, "out.bc");
	
	SPUExec(spu);
	
	//SPUDump(spu);
	
	SPUDtor(spu);
}

// int SPUExecute
