#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "instructions.h"

#include "bytecode.h"


int WriteBytecode(FILE* file, Memory* mem) {
	size_t written = 0;	
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
		printf("__func__ : Error writing to file (%zd/%u written)\n", written, mem->size);
		return -1;
	}
	
	return 0;
}


Memory* ReadBytecode(FILE* file) {
	unsigned magic = 0;
	fread(&magic, 1, sizeof(magic), file);
	if (memcmp(&magic, &MAGIC, sizeof(magic))) {
		printf("__func__: Magic number mismatch\n");
		return NULL;
	}
	
	unsigned version = 0;
	fread(&version, 1, sizeof(version), file);
	if (version != VERSION) {
		printf("__func__: Version mismatch (%u != %u)\n", version, VERSION);
		return NULL;
	}
	
	unsigned file_size = 0;
	fread(&file_size, 1, sizeof(file_size), file);
	if (!file_size) {
		printf("__func__: File size is 0\n");
		return NULL;
	}
	
	Memory* new_mem = MemCtor(file_size);
	if (!new_mem) {
		printf("__func__: Error constructing memory\n");
		return NULL;
	}
	
	
	size_t read = fread(new_mem->bytes, 1, file_size, file);
	if (read != file_size) {
		printf("__func__: Error reading file (%zd/%u read)\n", read, file_size);
		return NULL;
	}
	
	return new_mem;
} 
