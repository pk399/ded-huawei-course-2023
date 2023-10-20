#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "memory.h"
#include "error.h"

#include "tiny_onegin.h"


const unsigned DUMP_LIMIT = 50;


struct text6 {
	Memory* buf;
	Memory* lines;
};


unsigned _get_file_size(FILE* stream) {
	struct stat st = {};
	
	fstat(fileno(stream), &st);
	
	return st.st_size;	
}


Text* TextCtor(FILE* file) {
	assert(file);
	
	Text* txt = (Text*) calloc(1, sizeof(Text));
	if (!txt) {
		FATAL("Calloc returned NULL");
		return NULL;
	}
	
	if ( !(txt->buf = MemCtor(sizeof(char))) ) {
		FATAL("Error constructing memory for chars");
		free(txt);
		return NULL;
	}
	
	unsigned f_sz = _get_file_size(file);
	if ( MemResize(txt->buf, f_sz + 1) ) { // <---- F_SZ + 1
		FATAL("Error resizing memory");
		MemDtor(txt->buf);
		free(txt);
		return NULL;
	}
	
	unsigned read = fread(TextChars(txt), 1, f_sz, file);
	if (read != f_sz) {
		FATAL("Read less bytes than filesize");
		MemDtor(txt->buf);
		free(txt);
		return NULL;
	}
	
	if ( !(txt->lines = MemCtor(sizeof(char*))) ) {
		FATAL("Error constructing memory for lines");
		MemDtor(txt->buf);
		free(txt);
		return NULL;
	}
	
	char* tmp = TextChars(txt);
	MemWrite(txt->lines, &tmp);
	MemShift(txt->lines, 1);
	
	printf(BLUE("%s\n"), TextChars(txt));
	MemDump(txt->buf);
	
	for (unsigned j = 0 ; !MemEOF(txt->buf) && j < 150 ; MemShift(txt->buf, 1), j ++) {
		// printf("BIG LLOP %u %d\n", MemGetPt(txt->buf), MemEOF(txt->buf));
		// MemDump(txt->buf);
		char c = 0;
		MemRead(txt->buf, &c);
		if ( c == '\n' && TextChars(txt)[MemGetPt(txt->buf) + 1] ) {
			MemZero(txt->buf); // \n --> \0
			tmp = TextChars(txt) + MemGetPt(txt->buf) + 1;
			MemWrite(txt->lines, &tmp);
			MemShift(txt->lines, 1);
		}
	}
	
	if ( MemResize(txt->lines, MemGetPt(txt->lines)) ) {
		FATAL("Error resizing lines");
		TextDtor(txt);
		return NULL;
	}
	
	return txt;
}


int TextDump(Text* txt) {
	assert(txt);
	assert(txt->buf);
	assert(txt->lines);
	
	printf(GREEN("Text") "[" BLUE("%p") "] {\n", txt);
	
	printf( "\t" GREEN("Buffer") " - " );
	MemDump(txt->buf);
	
	printf( "\t" GREEN("Lines") " - " );
	MemDump(txt->lines);
	
	printf("\n");
	
	char* line = 0;
	for (unsigned i = 0; i < DUMP_LIMIT && ( line = TextGetLine(txt, i) ); i++)
		printf( "\t" YELLOW("%3u") " [" BLUE("%3ld") "]: %s\n", i, line - TextChars(txt), line);
	 
	printf("}\n");
	
	return 0;
}

	
int TextDtor(Text* txt) {
	if (txt) {
		MemDtor(txt->buf);
		MemDtor(txt->lines);
		free(txt);
	}
	
	return 0;
}


char* TextGetLine(Text* txt, unsigned index) {
	assert(txt);
	assert(txt->buf);
	assert(txt->lines);
	
	if ( MemEOFPt(txt->lines, index) )
		return NULL;
	
	return TextLines(txt)[index];
}


char* TextChars(Text* txt) {
	assert(txt);
	assert(txt->buf);
	
	return (char*) MemGetBf(txt->buf);
}


char** TextLines(Text* txt) {
	assert(txt);
	assert(txt->buf);
	assert(txt->lines);
	
	return (char**) MemGetBf(txt->lines);
}


unsigned TextLinesLen(Text* txt) {
	assert(txt);
	assert(txt->buf);
	assert(txt->lines);
	
	return (unsigned) MemGetSz(txt->lines);
}
