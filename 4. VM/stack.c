#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "colors.h"
#include "error.h"
#include "memory.h"

#include "stack.h"


struct _stack {
	Memory* mem;
};


Stack* StackCtor(unsigned elem_size) {
	Stack* stk = (Stack*) calloc(1, sizeof(Stack));
	if (!stk) {
		FATAL("Calloc returned NULL");
		return NULL;
	}
	
	if ( !(stk->mem = MemCtor(elem_size)) ) {
		FATAL("Error constructing memory");
		free(stk);
		return NULL;
	}

	return stk;
}


int StackDump(Stack* stk) {
	assert(stk);
	assert(stk->mem);

	printf(GREEN("Stack") "[" BLUE("%p") "] { ", stk);
	MemDump(stk->mem);
	printf("\b }\n");
	
	return 0;
}


int StackDtor(Stack* stk) {
	if (stk)
		if (stk->mem)
			RELAY( MemDtor(stk->mem) );
		free(stk);
		
	return 0;
}


int StackPush(Stack* stk, const void* value) {
	assert(stk);
	assert(stk->mem);
	
	MemShift(stk->mem, 1);
	RELAY( MemAResize(stk->mem) );
	RELAY( MemWrite(stk->mem, value) );
	
	return 0;
}


int StackPop(Stack* stk, void* value) {
	assert(stk);
	assert(stk->mem);
	
	RELAY( MemEOF(stk->mem) );
	
	MemRead(stk->mem, value);
	MemZero(stk->mem);
	MemShift(stk->mem, -1);
	RELAY( MemAResize(stk->mem) );
	
	return 0;
}
