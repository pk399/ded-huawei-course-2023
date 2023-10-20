#ifndef _INSTRUCTIONS_H_
#define _INSTRUCTIONS_H_

enum INSTRUCTIONS {
	#define DC(num, name, ...) CMD_ ## name = num,
	#include "commands.h"
	#undef DC
};

#endif /* _INSTRUCTIONS_H_ */
