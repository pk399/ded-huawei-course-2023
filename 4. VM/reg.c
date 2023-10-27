#include <stdio.h>

#include "error.h"

#include "reg.h"

const char* RegStr(unsigned reg_n) {
	#define DEF_REG(num, name) if (reg_n == num) { return #name; } else
	
	#include "reg_def.h"
	/* else */ if (0) ;
	
	FATAL("Reg doesn't exist!");
	return "DED";
}

