#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "tiny_onegin.h"


unsigned _get_file_size(FILE* stream) {
	struct stat st = {};
	
	fstat(fileno(stream), &st);
	
	return st.st_size;	
}


Text* TextCtor(FILE* file) {
	unsigned f_sz = _get_file_size(file);
	char* buf = (char*) calloc(1, f_sz + 1);
	assert(buf);
	
	unsigned read = fread(buf, 1, f_sz, file);
	assert(read == f_sz);
	
	Text* txt = (Text*) calloc(1, sizeof(Text));
	assert(txt);
	
	txt->buf = buf;
	
	unsigned lp = 0;
	txt->lines[lp++] = buf;
	for (unsigned i = 0; buf[i]; i++)
		if (buf[i] == '\n') {
			if (buf[i+1])
				txt->lines[lp++] = buf + i + 1;
			buf[i] = '\0';	
		}
	
	return txt;
}

	
int TextDtor(Text* txt) {
	free(txt->buf);
	free(txt);
	
	return 0;
}
