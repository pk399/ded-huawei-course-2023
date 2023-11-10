#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <float.h>

#include "reg.h"
#include "stack.h"
#include "colors.h"
#include "bcfile.h"
#include "error.h"
#include "opcode.h"

#include "spu.h"

// Note: SPU operates with doubles
// Note: opcodes are chars

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
	
	spu->call_stk = (Stack*) calloc(1, sizeof(Stack));
	if (
	    !spu->call_stk ||
	    STACKCTOR(spu->call_stk)
	   ) {
	    free(spu->code);
	    StackDtor(spu->stack);
	    StackDtor(spu->call_stk);
	    free(spu);
	    return NULL;
	}
	
	return spu;
}


int SPUDtor(SPU* spu) {
	if (spu) {
		StackDtor(spu->stack);
		StackDtor(spu->call_stk);
		free(spu->code);
		free(spu);
	}
	
	return 0;
}


int SPUDump(SPU* spu) {
	printf(GREEN("SPU") "[" BLUE("%p") "] {\n", spu);
	
	printf("\t Call stk: ");
	if (spu->call_stk->size) {
	    printf(">" YELLOW("%.2lf") "<", spu->call_stk->data[spu->call_stk->size - 1]);
	    
	    for (int i = spu->call_stk->size - 2; i >= 0 && spu->call_stk->size - i < DUMP_LIMIT; i--)
	        printf(" %.2lf", spu->call_stk->data[i]);
	} else {
	    printf("empty");
	}
	printf("\n");
	
	
	printf("\t Stack: ");
	if (spu->stack->size) {
	    printf(">" YELLOW("%.2lf") "<", spu->stack->data[spu->stack->size - 1]);
	    
	    for (int i = spu->stack->size - 2; i >= 0 && spu->stack->size - i < DUMP_LIMIT; i--)
	        printf(" %.2lf", spu->stack->data[i]);
	} else {
	    printf("empty");
	}
	printf("\n");
	
	printf("\t Registers:");
	for (unsigned i = 0; i < REG_COUNT && i < DUMP_LIMIT; i++)
	    if (spu->regs[i] < DBL_EPSILON) // TODO: replace with function
		    printf(" %s: %.2lf;", RegStr(i), spu->regs[i]);
		else
    		printf(" %s: " YELLOW("%.2lf") ";", RegStr(i), spu->regs[i]);
	printf("\n");
	
	unsigned shift = 0;
	if (spu->ip > DUMP_LIMIT/2)
	    shift = spu->ip - DUMP_LIMIT/2;
	
	printf("\t Code(ip = %u):", spu->ip);
	for (unsigned i = shift; i < spu->code_sz && i < DUMP_LIMIT + shift; i++)
	    if (i == spu->ip)
	        printf( YELLOW(" %2X"), spu->code[i] );
	    else
	        printf(" %2X", spu->code[i]);
	printf("\n");
	
	printf("\t Memory:\n");
	for (int i = 0; i < MEMORY_SIZE; i++) {
	    for (int j = 0; j < MEMORY_SIZE; j++) {
	        if (fabs(spu->memory[i*MEMORY_SIZE + j]) < DBL_EPSILON) {
	            printf("_");
	        } else {
	            printf(GREEN("#"));
	        }
	    }
	    printf("\n");
	}
	
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
	
	Arg_t    argt = OPGetArg(opcode);
	unsigned cmdt = OPGetCmd(opcode);
	
	double orig_arg = 0; // Just for holding memory
	double* arg = &orig_arg;
	
	// Note: This is a little bit strange,
	// Note: but here's how it works:
	// Note: 0 - NOP, 1 - RED, 2 - WRT
	// Note: Every RED is a NOP
	// Note: And every WRT is a RED and a NOP
	// Note: I think this is sensible
	int arg_level = 0; 

	// TODO: Check the case when argt.mem = 1 and argt.imm = argt.reg = 0
	if (argt.mem || argt.imm || argt.reg) {	
	    if (CodeGetArg(spu, arg)) HALT;
	    
	    arg_level = 1;
	}
	
	if (argt.reg) {
	    int num = *arg;
	    if (num < 0 || num >= REG_COUNT) HALT;
	    
	    arg = &(spu->regs[num]);
	    
	    arg_level = 2;
	}

	if (argt.mem) {
	    int num = *arg;
	    if (num < 0 || num >= MEMORY_SIZE * MEMORY_SIZE) HALT;
	    
	    arg = &(spu->memory[num]);
	    
	    arg_level = 2;
	}

	double to_push[MX_CMD_BUF] = {};
	double poped[MX_CMD_BUF] = {};
	
	
	#define NOP (arg_level >= 0)
	#define RDD (arg_level >= 1)
	#define WRT (arg_level >= 2)
	
	#define ARG (*arg)
	#define IP spu->ip
	#define CALLME(arg) if (StackPush(spu->call_stk, IP)) HALT; IP = (arg)
	#define REN double tmp = 0.0; if (StackPop(spu->call_stk, &tmp)) HALT; IP = tmp
		
	#warning ifs are slow
	#define DEF_CMD(NUM, NAME, ARG, POPS, PUSHS, ...) if (cmdt == NUM && ARG)                               \
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
	#undef REN
	#undef CALLME
	#undef IP
	#undef ARG
	#undef WRT
	#undef RDD
	#undef NOP
	#undef HALT
	
	
	return NOT_HALTED;
}


int SPUExec(SPU* spu) {
	while ( SPUStep(spu) == NOT_HALTED )
		;//SPUDump(spu);
		
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
