#include <stdlib.h>
#include <stdio.h>

#include "func_stack.h"
#include "colors.h"
#include "memory.h"
#include "bytecode.h"

#include "vm.h"

struct _vm {
	Stack* stack;
	char registers[4];
	Memory* code;
};


VM* VMCtor(unsigned mem_size) {
	VM* vm = (VM*) calloc(1, sizeof(VM));
	if (!vm) return vm;
	
	vm->stack = StackCtor();
	vm->code = MemCtor(mem_size);
	if (!vm->stack || !vm->code) return NULL;
	
	return vm;
}


int VMDtor(VM* vm) {
	StackDtor(vm->stack);
	MemDtor(vm->code);
	free(vm);
	
	return 0;
}


int VMDump(VM* vm) {
	printf(GREEN("VM") "[" BLUE("%p") "] {\n", vm);
	
	printf("\t");
	StackDump(vm->stack);
	
	printf("\t");
	MemDump(vm->code);
	
	printf("}\n");
	
	return 0;
}


int main() {
	VM* vm = VMCtor(10);
	
	abc[0].function(vm->stack, vm->registers, 3);
	
	VMDump(vm);
	
	char val = 'E';
	StackPop(vm->stack, &val);
	
	printf("Printing: %c\n", val);
	
	vm->code->bytes[0] = 'H';
	/*
	printf("Writing bytecode...\n");
	printf("version size: %zd %X\n", sizeof VERSION, VERSION);
	FILE* f = fopen("test.bc", "wb");
	WriteBytecode(f, vm->code);
	fclose(f);
	*/
	printf("Reading bytecode...\n");
	MemDtor(vm->code);
	
	FILE* f2 = fopen("test.bc", "rb");
	vm->code = ReadBytecode(f2);
	fclose(f2);
	
	VMDump(vm);
	
	VMDtor(vm);
}

// int VMExecute
