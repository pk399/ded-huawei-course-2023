#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "regs.h"
#include "stack.h"
#include "colors.h"
#include "bcfile.h"
#include "error.h"
#include "opcode.h"

#include "spu.h"

// SPU operates with doubles
// opcodes are chars

// SPUExecute(Memory* code)

SPU* SPUCtor() {
	SPU* spu = (SPU*) calloc(1, sizeof(SPU));
	if (!spu) return spu;
	
	spu->stack = (Stack*) calloc(1, sizeof(Stack));
	if (
	    !spu->stack ||
        STACKCTOR(spu->stack)    
	   ) {
        free(spu);
        return NULL;
    }
	
	spu->code = (char*) calloc(DEFAULT_CODE_SIZE, sizeof(char));
	spu->code_sz = DEFAULT_CODE_SIZE;
	if (!spu->code) {
	    StackDtor(spu->stack);
	    free(spu);
	    return NULL;
	}
	
	return spu;
}


int SPUDtor(SPU* spu) {
	if (spu) {
		StackDtor(spu->stack);
		free(spu->code);
		free(spu);
	}
	
	return 0;
}


int SPUDump(SPU* spu) {
	printf(GREEN("SPU") "[" BLUE("%p") "] {\n", spu);
	
	printf("\t Stack: ");
	if (spu->stack->size) {
	    printf(">" YELLOW("%.2lf") "<", spu->stack->data[0]);
	    
	    for (unsigned i = 1; i < spu->stack->size && i < DUMP_LIMIT; i++)
	        printf(" %.2lf", spu->stack->data[i]);
	} else {
	    printf("empty");
	}
	printf("\n");
	
	printf("\t Registers:");
	for (unsigned i = 0; i < REG_COUNT && i < DUMP_LIMIT; i++)
		printf(" %s: %.2lf;", RegStr(i), spu->regs[i]);
	printf("\n");
	
	unsigned shift = 0;
	if (spu->ip > DUMP_LIMIT/2)
	    shift = spu->ip - DUMP_LIMIT/2;
	
	printf("\t Code:");
	for (unsigned i = shift; i < spu->code_sz && i < DUMP_LIMIT + shift; i++)
	    if (i == spu->ip)
	        printf( YELLOW(" %2X"), spu->code[i] );
	    else
	        printf(" %2X", spu->code[i]);
	printf("\n");
	
	printf("}\n");
	
	return 0;
}


int SPULoad(SPU* spu, const char* filename) {
	assert(spu);
	assert(filename);

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
    
    return LoadBytecode(spu->code, &(spu->code_sz), file);
}


int CodeGetCmd(SPU* spu, char* cmd) {
    assert(spu);
    
    if (spu->ip >= spu->code_sz)
        return -1;
    
    *cmd = spu->code[spu->ip];
    spu->ip += 1;
    
    return 0;
}


int CodeGetArg(SPU* spu, double* arg) {
    assert(spu);
    
    if (spu->ip + sizeof(double) - 1 >= spu->code_sz)
        return -1;
    
    *arg = *((double*) (spu->code + spu->ip));
    
    spu->ip += sizeof(double);
    
    return 0;
}


STEP_RES SPUStep(SPU* spu) {
    #define HALT return HALTED

	char opcode = 0;
	if (CodeGetCmd(spu, &opcode)) HALT;
	
	ARG_TYPE argt = OPGetArg(opcode);
	unsigned cmdt = OPGetCmd(opcode);
	
	double arg = 0;
	double* reg = NULL;
	
	if (argt == IMM || argt == REG)
	    if (CodeGetArg(spu, &arg)) HALT;
	
	if (argt == REG) {
	    int num = arg;
	    if (num < 0 || num >= REG_COUNT) HALT;
	    
	    reg = &(spu->regs[num]);
	}
	
	double to_push[MX_CMD_BUF] = {};
	double poped[MX_CMD_BUF] = {};
	
	
	#define GREG (*reg)
		
	#define DEF_CMD(NUM, NAME, ARG, POPS, PUSHS, ...) if (cmdt == NUM && argt == ARG)                       \
											          {                                                     \
											              for (unsigned i = 0; i < POPS; i++) {             \
											                  int res = StackPop(spu->stack, &poped[i]);    \
											                  if (res) HALT;                                \
											              }                                                 \
												          __VA_ARGS__                                       \
												          for (unsigned i = 0; i < PUSHS; i++) {            \
											                  int res = StackPush(spu->stack, to_push[i]);  \
											                  if (res) HALT;                                \
											              }                                                 \
											          } else
	#include "cmd_def.h"
	
	/* else */ if (1) {
		FATAL("SPU reached default case!");
		HALT;
    }
	
	#undef DEF_CMD
	#undef GREG
	#undef HALT
	
	return NOT_HALTED;
}


int SPUExec(SPU* spu) {
	while ( SPUStep(spu) == NOT_HALTED )
		SPUDump(spu);
		
	return 0;
}


int main() {
	SPU* spu = SPUCtor();
	
	SPUDump(spu);
	
	printf("Reading bytecode...\n");
	SPULoad(spu, "out.bc");
	
	SPUExec(spu);
	
	SPUDump(spu);
	
	SPUDtor(spu);
}
