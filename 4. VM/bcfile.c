#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "error.h"

#include "bcfile.h"


int WriteBytecode(FILE* file, char* arr, unsigned sz) {
	assert(file);
	assert(arr);

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
	
	written = fwrite(&sz, sizeof(sz), 1, file);
	if (written != 1) {
		FATAL("Error writing size to file");
		return -1;
	}
	
	written = fwrite(arr, sizeof(char), sz, file);
	if (written != sz) {
		FATAL("Error writing to file");
		//printf("__func__ : Error writing to file (%u/%u written)\n", written, mem->size);
		// TODO: have parameters in FATAL macro
		return -1;
	}
	
	return 0;
}


int LoadBytecode(char* arr, unsigned* sz, FILE* file) {
	assert(file);
	assert(arr);
    
    unsigned read = 0;
    
	unsigned magic = 0;
	read = fread(&magic, sizeof(magic), 1, file);
	if (read != 1 || memcmp(&magic, &MAGIC, sizeof(magic))) {
		FATAL("Magic number mismatch");
		return -1;
	}
	
	unsigned version = 0;
	read = fread(&version,sizeof(version), 1, file);
	if (read != 1 || version != VERSION) {
		FATAL("Version mismatch");
		//printf("__func__: Version mismatch (%u != %u)\n", version, VERSION);
		// TODO: FATAL params
		return -1;
	}
	
	unsigned file_size = 0;
	read = fread(&file_size,  sizeof(file_size), 1, file);
	if (read != 1 || !file_size) {
		FATAL("File size is 0");
		return -1;
	}
	
	if ( realloc(arr, file_size) ) return (FATAL("Error resizing array"), -1);
	*sz = file_size;
	
	read = fread(arr, sizeof(char), file_size, file);
	if (read != file_size) {
		FATAL("Error reading file");
		//printf("__func__: Error reading file (%u/%u read)\n", read, file_size);
		// TODO: FATAL params
		return -1;
	}
	
	return 0;
}
