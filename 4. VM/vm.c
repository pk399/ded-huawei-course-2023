#include <stdlib.h>
#include <stdio.h>

#include "func_stack.h"
#include "colors.h"
#include "memory.h"
#include "bytecode.h"
#include "instructions.h"
#include "cmd.h"

#include "vm.h"

struct _vm {
	Stack* stack;
	char registers[4];
	Memory* code;
};


VM* VMCtor() {
	VM* vm = (VM*) calloc(1, sizeof(VM));
	if (!vm) return vm;
	
	vm->stack = StackCtor();
	vm->code = MemCtor();
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
	
	printf("\t" GREEN("Registers "));
	for (unsigned i = 0; i < sizeof(vm->registers); i++)
		printf("%02d ", (int) vm->registers[i]);
	printf("\n");
	
	printf("\t");
	MemDump(vm->code);
	
	printf("}\n");
	
	return 0;
}


int VMLoad(VM* vm, const char* filename) {
	FILE* f = fopen(filename, "rb");
	if (!f) {
		printf("VMLoad: Error opening file\n");
		return -1;
	}
	
	int res = fVMLoad(vm, f);
	
	fclose(f);
	
	return res;
}


int fVMLoad(VM* vm, FILE* file) {
	return ReadBytecodeMem(vm->code, file);
}


int VMStep(VM* vm) {
	char cmd = 0;
	if ( MemRead(vm->code, &cmd) ) return -1;
	
	switch ( CmdI(cmd) ) {
		case HLT:
			return -1;
		
		case PUSH:
			{
				char param = 0;
				if ( MemRead(vm->code, &param) ) return -1;
				
				switch ( CmdPar(cmd) ) {
					case LITERAL:
						if ( StackPush(vm->stack, param) ) return -1;
					
						break;
						
					case REGISTER:
						{
							unsigned char par_reg = param;
							if ( par_reg >= sizeof(vm->registers) ) {
								printf("VMStep: push outside of registers range\n");
								return -1;
							}
							
							if ( StackPush(vm->stack, vm->registers[par_reg]) ) return -1;
						}
						break;
						
					default:
						printf("VMStep: push reached default case\n");
				}
			}
			break;
		
		case POP:
			{
				char param = 0;
				if ( MemRead(vm->code, &param) ) return -1;
				
				switch ( CmdPar(cmd) ) {
					case LITERAL:
						printf("VMStep: tried popping a literal\n");
						return -1;
						
					case REGISTER:
						{
							unsigned char par_reg = param;
							if ( par_reg >= sizeof(vm->registers) ) {
								printf("VMStep: pop outside of registers range\n");
								return -1;
							}
							
							if ( StackPop(vm->stack, &(vm->registers[par_reg])) ) return -1;
						}
						break;
						
					default:
						printf("VMStep: pop reached default case\n");
				}
			}
			break;
			
		case IN:
			{
				printf( BLUE("VM is waiting for input: ") );
				int val = 0;
				if ( scanf("%d", &val) != 1) {
					printf("Scanf error\n");
					return -1;
				}
				if ( StackPush(vm->stack, (char) val) ) return -1;
			}
			break;
		
	 	case OUT:
	 		{
		 		char val = 0;
		 		if ( StackPop(vm->stack, &val) ) return -1;
		 		printf( MAGENTA("%d\n"), (int) val );
		 	}
	 		break;
	 	
		case ADD:
			{
				char val1 = 0;
				char val2 = 0;
		 		if ( StackPop(vm->stack, &val1) ) return -1;
		 		if ( StackPop(vm->stack, &val2) ) return -1;
		 		if ( StackPush(vm->stack, val1 + val2) ) return -1;
		 	}
	 		break;
	 		
		case SUB:
			{
				char val1 = 0;
				char val2 = 0;
		 		if ( StackPop(vm->stack, &val1) ) return -1;
		 		if ( StackPop(vm->stack, &val2) ) return -1;
		 		if ( StackPush(vm->stack, val1 - val2) ) return -1;
		 	}
	 		break;
		
		case MUL:
			{
				char val1 = 0;
				char val2 = 0;
		 		if ( StackPop(vm->stack, &val1) ) return -1;
		 		if ( StackPop(vm->stack, &val2) ) return -1;
		 		if ( StackPush(vm->stack, val1 * val2) ) return -1;
		 	}
	 		break;
		
		case DIV:
			{
				char val1 = 0;
				char val2 = 0;
		 		if ( StackPop(vm->stack, &val1) ) return -1;
		 		if ( StackPop(vm->stack, &val2) ) return -1;
		 		if ( StackPush(vm->stack, val1 / val2) ) return -1;
		 	}
	 		break;
		
		case SQRT:
		case SIN:
		case COS:
			printf( YELLOW("Sqrt, Sin, Cos are not implemented for now, because we have integers") );
			break;
		
		default:
			printf("VMStep: reached default case\n");
			return -1;
	}
	
	return 0;
}


int VMExec(VM* vm) {
	while ( !VMStep(vm) )
		VMDump(vm);
		
	return 0;
}


int main() {
	VM* vm = VMCtor();
	
	VMDump(vm);
	
	printf("Reading bytecode...\n");
	VMLoad(vm, "out.bc");
	
	VMExec(vm);
	
	VMDump(vm);
	
	VMDtor(vm);
}

// int VMExecute
