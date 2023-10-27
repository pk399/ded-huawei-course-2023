#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

#include "bcfile.h"
#include "error.h"
#include "opcode.h"


const unsigned CODE_SZ = 1000000; // One million dh
const unsigned LINE_SZ = 1000;


// Global vars
char code[CODE_SZ] = {};
unsigned ip = 0;


// https://stackoverflow.com/questions/5820810/case-insensitive-string-comparison-in-c
int MyStricmp(const char* a, const char* b) {
    for ( ; ; a++, b++) {
        int d = tolower(*a) - tolower(*b);
        if (d != 0 || !*a)
            return d;
    }
}


double ParseParameter(char* str, ARG_TYPE *argt) {
	assert(str);
	assert(strlen(str));
	assert(argt);
	
	#define DEF_REG(num, name) if ( !MyStricmp(str, #name) ) { \
	                               *argt = REG;                \
	                               return num;                 \
	                           } else
    
    #include "reg_def.h"
    #undef DEF_REG
    /* else */ if (0) ;   
    	
	*argt = IMM;
	return atof(str);
}


int CodeWriteOP(char op) {
    if ( ip >= CODE_SZ )
        return -1;
    
    code[ip++] = op;
    
    return 0;
}


int CodeWritePar(double par) {
    if ( ip + sizeof(double) - 1 >= CODE_SZ)
        return -1;
    
    *((double*) (code + ip)) = par;
    ip += sizeof(double);
    
    return 0;
}


void ParseFile(FILE* file) {
	char line[LINE_SZ] = {};
	for (unsigned i = 0; fgets(line, LINE_SZ, file); i++) {
		
		printf("Encountered line %u: |%s|\n", i, line);
	
		char* comment = strchr(line, ';');
		if (comment)
			*comment = '\0';
			
		printf("After removing comment: |%s|\n", line);
			
		char* cmd_str = strtok(line, " \t\n");
		if (!cmd_str)
			continue;
		
		printf("Command: |%s|\n", cmd_str);
		
		ARG_TYPE argt = NOP;

		char* param_str = strtok(NULL, " \t\n");
		printf("Parameter: |%s|\n", param_str);
		double param = 0.0;
		
		if (param_str) {
		    printf("Decided to parse parameter\n");
			param = ParseParameter(param_str, &argt);
	    }
		
		#define DEF_CMD(NUM, NAME, ARG, ...) if ( !MyStricmp(cmd_str, #NAME) && argt == ARG ) { \
		                                        printf("Encountered %s(%s)\n", #NAME, #ARG);   \
                                                CodeWriteOP(OPCtor(NUM, argt));                \
										        if (argt != NOP)                               \
										            CodeWritePar(param);                       \
										    } else
		
		#include "cmd_def.h"
		#undef DEF_CMD
		/* else */ if (1) {
			//WARNING("No match for on %s(%d) line %u was found!", cmd_str, argt, i+1);
			WARNING("No match for command was found!");
		}
	}
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
	
	ParseFile(in_file);
	
	FILE* out_file = NULL;
	if (argc < 3) {
		out_file = fopen("out.bc", "wt");
printf("Picking out.bc\n");
	} else
		out_file = fopen(args[2], "wt");
	if (!out_file)
		return -printf("Failed to open file for writing\n");
	
	
	ip = (ip > CODE_SZ) ? CODE_SZ : ip;
	WriteBytecode(out_file, code, ip);
	
	fclose(in_file);
	fclose(out_file);
		
	return 0;
}
