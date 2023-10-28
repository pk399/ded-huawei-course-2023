#include "bytecode.h"
#include "memory"
#include <stdio.h>

int main() {
	FILE* f = fopen("out.bc", "rt");
	Memory* code = ReadBytecode(f);
	
	MemDump(code);
	
	fclose(f);
}
