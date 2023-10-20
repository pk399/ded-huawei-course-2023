#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "memory.h"
#include "instructions.h"
#include "bytecode.h"
#include "error.h"

#include "bcfile.h"


int WriteBytecode(FILE* file, Memory* mem) {
	assert(file);
	assert(mem);
	
	if ( mem->elem_size != sizeof(code_word) ) {
		FATAL("Memory elem_size must hold code_word (from bytecode.h)");
		return -1;
	}

	unsigned written = 0;	
	written = fwrite(&MAGIC, sizeof(MAGIC), 1, file);
	if (written != 1) {
		FATAL("Error writing magic to file");
		return -1;
	}
	
	written = fwrite(&VERSION, sizeof(VERSION), 1, file);
	if (written != 1) {
		FATAL("Error writing version to file");
		return -1;
	}
	
	written = fwrite(&mem->size, sizeof(mem->size), 1, file);
	if (written != 1) {
		FATAL("Error writing size to file");
		return -1;
	}
	
	written = fwrite(mem->bytes, sizeof(code_word), mem->size, file);
	if (written != mem->size) {
		FATAL("Error writing to file");
		//printf("__func__ : Error writing to file (%u/%u written)\n", written, mem->size);
		// TODO: have parameters in FATAL macro
		return -1;
	}
	
	return 0;
}


int ReadBytecodeMem(Memory* mem, FILE* file) {
	assert(file);
	assert(mem);
	
	if ( mem->elem_size != sizeof(code_word) ) {
		FATAL("Memory elem_size must == sizeof code_word (from bytecode.h)");
		return -1;
	}

	unsigned magic = 0;
	fread(&magic, 1, sizeof(magic), file);
	if (memcmp(&magic, &MAGIC, sizeof(magic))) {
		FATAL("Magic number mismatch");
		return -1;
	}
	
	unsigned version = 0;
	fread(&version, 1, sizeof(version), file);
	if (version != VERSION) {
		FATAL("Version mismatch");
		//printf("__func__: Version mismatch (%u != %u)\n", version, VERSION);
		// TODO: FATAL params
		return -1;
	}
	
	unsigned file_size = 0;
	fread(&file_size, 1, sizeof(file_size), file);
	if (!file_size) {
		FATAL("File size is 0");
		return -1;
	}
	
	if ( MemResize(mem, file_size) ) return (FATAL("Error resizing memory"), -1);
	
	unsigned read = fread(mem->bytes, sizeof(code_word), file_size, file);
	if (read != file_size) {
		FATAL("Error reading file");
		//printf("__func__: Error reading file (%u/%u read)\n", read, file_size);
		// TODO: FATAL params
		return -1;
	}
	
	return 0;
}


Memory* ReadBytecode(FILE* file) {
	assert(file);

	Memory* new_mem = MemCtor(sizeof(code_word));
	if (!new_mem) {
		FATAL("Error constructing memory");
		return NULL;
	}
	
	if ( ReadBytecodeMem(new_mem, file) ) {
		MemDtor(new_mem);
		return NULL;
	}
	
	return new_mem;
} 
