#include <stdlib.h>
#include <stdio.h>

#include "colors.h"

#include "memory.h"

Memory* MemCtor(unsigned size) {
	printf("Ctorring that mem\n");
	Memory* new_mem = (Memory*) calloc(1, sizeof(Memory));
	printf("CALLOCED\n");
	if (!new_mem) return NULL;
printf("GON REALL\n");
	MemRealloc(new_mem, size);
	printf("EXIT %p %pc\n", new_mem, new_mem->bytes);
	if (!new_mem->bytes) {
		free(new_mem);
		return NULL;
	}
	printf("%p", new_mem);
	return new_mem;
}


int MemDump(Memory* mem) {
	printf(GREEN("Memory") "[" BLUE("%p") "] { ", mem);
	
	for (unsigned i = 0; i < mem->size && i < DUMP_LIMIT; i++) {
		printf("%02X ", (unsigned) mem->bytes[i]);
	}
	
	printf("}\n");
	
	return 0;
}


int MemDtor(Memory* mem) {
	free(mem->bytes);
	free(mem);
	
	return 0;
}


int MemRealloc(Memory* mem, unsigned new_size) {
	int non_zero = 0;	
	
	if (mem->bytes && new_size < mem->size)
		for (unsigned i = new_size; i < mem->size; i++)
			non_zero += mem->bytes[i];
	
	if (non_zero)
		printf("MemRealloc: Warning new size is less than current size, and there is some non-zero elements\n");
		
	if (mem->p >= new_size) {
		printf("MemRealloc: Warning: pointer is greater than new size, shifting\n");
		mem->p = new_size - 1;
	}
	
	mem->bytes = (char*) realloc(mem->bytes, new_size);
	
	for (unsigned i = mem->size; i < new_size; i++)
		mem->bytes[i] = 0;
	
	mem->size = new_size;
	
	return 0;
}


int MemRead(Memory* mem) {
	if (mem->p >= mem->size) {
		printf("MemRead: Out of dounds\n");
		return -1;
	}
	
	return mem->bytes[mem->p++];
}


int MemWrite(Memory* mem, char byte) {
printf("MEMRITE %p\n", mem);
	if (mem->p >= mem->size) {
		printf("MemWrite: Out of goungos\n");
		return -1;
	}
	printf("MEMRITE\n");
	
	if (mem->p == mem->size - 1)
		MemRealloc(mem, mem->size * 2);
	
	return (mem->bytes[mem->p++] = byte);
}


int MemSeek(Memory* mem, unsigned position) {
	if (position >= mem->size) {
		printf("MemSeek: Position out of rounds");
		return -1;
	}
	
	mem->p = position;	
	
	return 0;	
}
