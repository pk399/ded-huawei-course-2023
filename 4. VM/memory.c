#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "error.h"

#include "memory.h"

typedef struct _memory {
	void* bytes;
	unsigned elem_size;
	unsigned size;
	unsigned pointer;
} Memory;

const unsigned GROW_BY = 2;
// const unsigned SHRINK_WHEN = 3;
// const unsigned SHRINK_BY = 2;

const unsigned DEFAULT_SIZE = 512;
const unsigned DUMP_LIMIT = 20;

Memory* MemCtor(unsigned elem_size) {
	assert(elem_size);

	Memory* mem = (Memory*) calloc(1, sizeof(Memory));
	if (mem == NULL) {
		FATAL("Calloc returned NULL");
		return NULL;
	}
	
	mem->elem_size = elem_size;
	mem->size = 0;
	mem->pointer = 0;
	
	return mem;
}


int MemDump(Memory* mem) {
	assert(mem);
	assert(mem->elem_size);

	printf(GREEN("Memory") "[" BLUE("%p") "] ", mem);
	
	unsigned offset = 0;
	if (mem->pointer > DUMP_LIMIT/2)
		offset = mem->pointer - DUMP_LIMIT/2;
	
	if (mem->bytes) {
		printf("Data[" BLUE("%p") "] = { ", mem->bytes);
		for (unsigned i = offset; i < mem->size && i < DUMP_LIMIT + offset; i++)
			for (unsigned j = 0; j < mem->elem_size; j++)
				if (i == mem->pointer)
					printf(YELLOW("%02X "), ((char*) mem->bytes)[i * mem->elem_size + j]);
				else
					printf("%02X ", ((char*) mem->bytes)[i * mem->elem_size + j]);
		printf("}, Pointer = " YELLOW("%u") "\n", mem->pointer);
	} else
		printf( "Data[" BLUE("NULL") "]\n" );
	
	return 0;
}


int MemDtor(Memory* mem) {
	if (mem) {
		if (mem->bytes)
			free(mem->bytes);
		free(mem);
	}
	
	return 0;
}


int MemResize(Memory* mem, unsigned new_size) {
	assert(mem);
	assert(mem->elem_size);

	if (new_size == 0) {
		if (mem->bytes)
			free(mem->bytes);
		return 0;
	}
		
	if (mem->bytes && new_size < mem->size) {
		int non_zero = 0;
	
		for (unsigned i = new_size; i < mem->size; i++)
			for (unsigned j = 0; j < mem->elem_size; j++)
				non_zero += ((char*) mem->bytes)[i * mem->elem_size + j];
	
		if (non_zero)
			WARNING("New size is less than current size and there is some non-zero elements");
	}
	
	mem->bytes = realloc(mem->bytes, new_size*mem->elem_size);
	
	if (!mem->bytes) {
		FATAL("Realloc returned null");
		return -1;
	}
	
	if (mem->pointer >= new_size) {
		WARNING("Pointer is greater than new size. Shifting");
		mem->pointer = new_size - 1;
	}
	
	for (unsigned i = mem->size; i < new_size; i++)
		for (unsigned j = 0; j < mem->elem_size; j++)
			((char*) mem->bytes)[i * mem->elem_size + j] = 0;
	
	mem->size = new_size;
	
	return 0;
}


int MemRead(Memory* mem, void* elem) {
	assert(mem);
	assert(mem->elem_size);
	assert(elem);

	if ( MemEOF(mem) ) {
		FATAL("Out of dounds");
		return -1;
	}
	
	memcpy(elem, ((char*) mem->bytes) + (mem->pointer++ * mem->elem_size), mem->elem_size);
	
	return 0;
}


int MemWrite(Memory* mem, const void* elem) {
	assert(mem);
	assert(mem->elem_size);
	assert(elem);
	
	if (mem->size == 0)
		MemResize(mem, DEFAULT_SIZE);
	
	if ( MemEOF(mem) ) {
		FATAL("Out of goungos");
		return -1;
	}
	
	memcpy(((char*) mem->bytes) + (mem->pointer * mem->elem_size), elem, mem->elem_size);
	
	RELAY( MemShift(mem, 1) );
		
	return 0;
}


int MemEOF(Memory* mem) {
	assert(mem);
	assert(mem->elem_size);
	
	return (mem->pointer >= mem->size);
}


int MemShift(Memory* mem, int position_delta) {
	assert(mem);
	assert(mem->elem_size);
	
	//printf("ptr + delta: %d\n", mem->pointer + position_delta);
	if ((int) mem->pointer + position_delta < 0) {
		FATAL("Shifting will result in a negative pointer");
		return -1;
	}
	
	unsigned new_ptr = mem->pointer + position_delta;
	
	RELAY( MemSeek(mem, new_ptr) );
	return 0;
}


int MemSeek(Memory* mem, unsigned new_position) {
	if (new_position >= mem->size) {
		FATAL("Position out of rounds");
		return -1;
	}
	
	mem->pointer = new_position;	
	
	return 0;	
}


void* MemGetBf(Memory* mem) {
	return mem->bytes;
}


unsigned MemGetSz(Memory* mem) {
	return mem->size;
}


unsigned MemGetPt(Memory* mem) {
	return mem->pointer;
}
