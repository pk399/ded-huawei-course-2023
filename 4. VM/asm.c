#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

#include "instructions.h"
#include "memory.h"
#include "tiny_onegin.h"
#include "bcfile.h"
#include "bytecode.h"
#include "error.h"


void StrToUpper(char* str) {
	for (unsigned i = 0; str[i]; i++)
		str[i] = toupper(str[i]);
}


code_word ParseParameter(char* str, PAR_T *pt) {
	assert(str);
	assert(strlen(str));
	
	StrToUpper(str);
	printf("Parsing parameter |%s|\n", str);
	
	code_word arg = {0};
	
	if (strlen(str) == 3 && str[0] == 'R', str[2] == 'X') {
		*pt = REGISTER;
		arg.iarg = str[1] - 'A';
		return arg;
	}
	
	*pt = LITERAL;
	arg.iarg = atoi(str);
	return arg;
}


void ParseFile(Memory* code, FILE* file) {
	Text* txt = TextCtor(file);
	
	char* line = 0;
	for (unsigned i = 0; line = TextGetLine(txt, i); i++) {
		
		printf("Encountered line: |%s|\n", line);
	
		char* comment = strchr(line, ';');
		if (comment)
			*comment = '\0';
			
		printf("After removing comment: |%s|\n", line);
			
		if (!strlen(line))
			continue;
			
		char* word = strtok(line, " \t\n");
		if (!word)
			continue;
		
		StrToUpper(word);
		printf("Word: %s\n", word);
		
		#define DC(num, name, par, ...) if (!strcmp(word, #name)) {                                \
											printf("Encountered %s\n", #name);                     \
											                                                       \
											PAR_T pt = NONE;									   \
																								   \
											code_word arg = {0};								   \
											if (par)                                               \
												arg = ParseParameter( strtok(NULL, " \t\n"), &pt); \
																								   \
											code_word ins = CWCtor(CMD_ ## name, pt);			   \
											MemWrite(code, &ins);								   \
											MemShift(code, 1);									   \
											MemAResize(code);									   \
																								   \
											if (par) {									           \
												MemWrite(code, &arg);                              \
												MemShift(code, 1);                                 \
												MemAResize(code);                                  \
											}                                                      \
										} else
		
		#include "commands.h"
		#undef CD
		{
			WARNING("Instruction doesn't exist");
		}
		
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
		
	Memory* code = MemCtor(CW_SZ);
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
	
	MemResize(code, code->pointer);
	WriteBytecode(out_file, code);
	
	MemDtor(code);
	fclose(in_file);
	fclose(out_file);
		
	return 0;
}
