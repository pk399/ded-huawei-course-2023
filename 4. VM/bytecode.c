#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "instructions.h"

#include "bytecode.h"


int WriteBytecode(FILE* file, Memory* mem) {
	unsigned written = 0;	
	written = fwrite(&MAGIC, sizeof(MAGIC), 1, file);
	if (written != 1) {
		printf("__func__: Error writing magic to file\n");
		return -1;
	}
	
	written = fwrite(&VERSION, sizeof(VERSION), 1, file);
	if (written != 1) {
		printf("__func__: Error writing version to file\n");
		return -1;
	}
	
	written = fwrite(&mem->size, sizeof(mem->size), 1, file);
	if (written != 1) {
		printf("__func__: Error writing size to file\n");
		return -1;
	}
	
	written = fwrite(mem->bytes, 1, mem->size, file);
	if (written != mem->size) {
		printf("__func__ : Error writing to file (%u/%u written)\n", written, mem->size);
		return -1;
	}
	
	return 0;
}


int ReadBytecodeMem(Memory* mem, FILE* file) {
	unsigned magic = 0;
	fread(&magic, 1, sizeof(magic), file);
	if (memcmp(&magic, &MAGIC, sizeof(magic))) {
		printf("__func__: Magic number mismatch\n");
		return -1;
	}
	
	unsigned version = 0;
	fread(&version, 1, sizeof(version), file);
	if (version != VERSION) {
		printf("__func__: Version mismatch (%u != %u)\n", version, VERSION);
		return -1;
	}
	
	unsigned file_size = 0;
	fread(&file_size, 1, sizeof(file_size), file);
	if (!file_size) {
		printf("__func__: File size is 0\n");
		return -1;
	}
	
	if ( MemRealloc(mem, file_size) ) return (printf("__func__: Error reallocating memory\n"), -1);
	
	unsigned read = fread(mem->bytes, 1, file_size, file);
	if (read != file_size) {
		printf("__func__: Error reading file (%u/%u read)\n", read, file_size);
		return -1;
	}
	
	return 0;
}


Memory* ReadBytecode(FILE* file) {
	Memory* new_mem = MemCtor(333);
	if (!new_mem) {
		printf("__func__: Error constructing memory\n");
		return NULL;
	}
	
	if ( ReadBytecodeMem(new_mem, file) ) return NULL;
	
	return new_mem;
} 
