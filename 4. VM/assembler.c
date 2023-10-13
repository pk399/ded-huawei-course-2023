#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

#include "instructions.h"
#include "memory.h"
#include "tiny_onegin.h"
#include "cmd.h"
#include "bytecode.h"


void StrToLower(char* str) {
	for (unsigned i = 0; str[i]; i++)
		str[i] = tolower(str[i]);
}


char ParseParameter(char* str, enum PARAM_TYPE *type) {
	assert(str);
	assert(strlen(str));
	
	StrToLower(str);
	printf("Parsing parameter |%s|\n", str);
	
	
	if (strlen(str) == 3 && str[0] == 'r', str[2] == 'x') {
		*type = REGISTER;
		return str[1] - 97;
	}
	
	*type = LITERAL;
	return (char) atoi(str);
}


#define SIZEARR(t) (sizeof(t)/sizeof(t[0]))


void ParseFile(Memory* code, FILE* file) {
	Text* txt = TextCtor(file);
	
	for (unsigned i = 0; txt->lines[i]; i++) {
		char* line = txt->lines[i];
		
		printf("Encountered line: |%s|\n", line);
	
		char* comment = strchr(line, ';');
		if (comment)
			*comment = '\0';
			
		printf("After removing comment: |%s|\n", line);
			
		if (!strlen(line))
			continue;
			
		char* word = strtok(line, " \t\n");
		StrToLower(word);
		printf("Word: %s\n", word);
		
		unsigned j = 0;
		for ( ; j < SIZEARR(INAMES); j++) {
			printf("J loop %u; %s\n", j, INAMES[j]);
			if (!strcmp(word, INAMES[j])) {
				if (!strcmp(word, "push")) {
					enum PARAM_TYPE pt = (enum PARAM_TYPE) 0;
					char val = ParseParameter(strtok(NULL, " \t\n"), &pt);
					
					MemWrite(code, CmdCtorPar(PUSH, pt));
					MemWrite(code, val);
				} else if (!strcmp(word, "pop")) {
					enum PARAM_TYPE pt = (enum PARAM_TYPE) 0;
					char val = ParseParameter(strtok(NULL, " \t\n"), &pt);
					assert(pt == REGISTER);
					
					MemWrite(code, CmdCtorPar(POP, pt));
					MemWrite(code, val);
				} else
					MemWrite(code, CmdCtor( (enum INSTRUCTIONS) j ));
				
				break;
			}
		}
		
		assert(j != SIZEARR(INAMES));
	}
	
	TextDtor(txt);
}


int main(int argc, char** args) {
printf("123123");
	if (argc < 2)
		return -printf("Usage: %s <ASM file> [Output file]\n", args[0]);
	
	printf("ARgc: %d", argc);
	printf("ARGS: %s\n", args[1]);
	
	FILE* in_file = fopen(args[1], "rt");
	if (!in_file)
		return -printf("Failed to open input file (%s)\n", args[1]);
		
	Memory* code = MemCtor();
	assert(code); // SegFault ?! No, u just stupid
	
	ParseFile(code, in_file);
	
	FILE* out_file = NULL;
	if (argc < 3) {
		out_file = fopen("out.bc", "wt");
		printf("Picking out.b\n");
	} else
		out_file = fopen(args[2], "wt");
	if (!out_file)
		return -printf("Failed to open file for writing\n");
	
	MemRealloc(code, code->p);
	WriteBytecode(out_file, code);
	
	MemDtor(code);
	fclose(in_file);
	fclose(out_file);
		
	return 0;
}
