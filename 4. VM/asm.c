#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

#include "bcfile.h"
#include "error.h"
#include "opcode.h"
// Note: no focus on performance 

const unsigned CODE_SZ = 1000000; // One million dh
const unsigned LINE_SZ = 1000;
const unsigned MARKER_SZ = 1000;


struct Marker {
    char* name;
    int pointer;
};


// Global vars (easy to implement)
char code[CODE_SZ] = {};
unsigned ip = 0;
Marker markers[MARKER_SZ] = {};
unsigned mp = 0;


// https://stackoverflow.com/questions/5820810/case-insensitive-string-comparison-in-c
int MyStricmp(const char* a, const char* b) {
    for ( ; ; a++, b++) {
        int d = tolower(*a) - tolower(*b);
        if (d != 0 || !*a)
            return d;
    }
}


int MarkerInsert(char* marker) {
    assert(marker);
    
    printf("Inserting marker %s(%u)\n", marker, ip);
    
    if (mp >= MARKER_SZ)
        return -1;
    
    char* str = (char*) malloc(strlen(marker) + 1);
    if (!str) return -1;
    strcpy(str, marker);
    
    markers[mp++] = (Marker) {str, (int) ip};
    
    return 0;
}


int MarkerFind(char* marker) {
    for (unsigned i = 0; i < mp; i++) {
        Marker m = markers[i];
        if ( !strcmp(m.name, marker) ) {
            printf("Found marker %s(%d)\n", m.name, m.pointer);
            return m.pointer;
        }
    }
    
    return -1;
}


double ParseParameter(char* str, Arg_t *argt) {
	assert(str);
	assert(strlen(str));
	assert(argt);
	
	if (str[0] == '[') {
	    argt->mem = 1;
	    str += 1;
	    char* bracket_pos = strchr(str, ']');
	    if (bracket_pos) *bracket_pos = '\0';
	    else /* do_nothing() */;
	    
	    printf("Encountered the memory arg...\n");
	    printf("Here's without braces: |%s|\n", str);
	}
	
	
	#define DEF_REG(num, name) if ( !MyStricmp(str, #name) ) { \
	                               argt->reg = 1;              \
	                               return num;                 \
	                           } else
    
    #include "reg_def.h"
    #undef DEF_REG
    /* else */ if (0) ; 
    
    argt->imm = 1;
    
    int mrk = MarkerFind(str);
    if ( mrk != -1 ) {
        return mrk;
    }
    
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
	    
	    char* marker = strchr(line, ':');
	    if (marker) { // Has marker
	        printf("Parsing as marker\n");
	        *marker = '\0';
	        if ( MarkerInsert(strtok(line, " \t\n")) ) {
	            FATAL("Could not insert marker!!!\n");
	        }
	        
	        continue;
	    }
	    
		char* cmd_str = strtok(line, " \t\n");
		if (!cmd_str)
			continue;
		
		printf("Command: |%s|\n", cmd_str);
		
		Arg_t argt = { };

		char* param_str = strtok(NULL, " \t\n");
		printf("Parameter: |%s|\n", param_str);
		double param = 0.0;
		
		if (param_str) {
		    printf("Decided to parse parameter\n");
			param = ParseParameter(param_str, &argt);
	    }
		
		// TODO: Maybe not just literally ignore the arg type
		#define DEF_CMD(NUM, NAME, ...) if ( !MyStricmp(cmd_str, #NAME) ) {                    \
		                                        printf("Encountered %s(type)\n", #NAME);       \
                                                CodeWriteOP(OPCtor(NUM, argt));                \
										        if (argt.imm || argt.reg)                      \
										            CodeWritePar(param);                       \
										    } else
		
		#include "cmd_def.h"
		#undef DEF_CMD
		/* else */ if (1) {
			//WARNING("No match for on %s(%d) line %u was found!", cmd_str, argt, i+1);
			WARNING("No match for command was found!");
			#warning error // <- TODO: throw some error or something
		}
	}
}


int main(int argc, char** args) {
printf("123123");
	if (argc < 2)
		return -printf("Usage: %s <ASM file> [Output file]\n", args[0]);
	
printf("ARgc: %d", argc);
printf("ARGS: %s\n", args[1]);
	
	for (int i = 0; i < 2; i++) {
	    ip = 0;
	    printf("===========\n");
	    printf("   PASS %d \n", i + 1);	    
	    printf("===========\n");	    
	
	    FILE* in_file = fopen(args[1], "rt");
	    if (!in_file)
		    return -printf("Failed to open input file (%s)\n", args[1]);
	    
	    ParseFile(in_file);
	    #warning do not reopen
	    
	    fclose(in_file);
	}
	
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
	
	fclose(out_file);
		
	return 0;
}
