#ifndef _CMD_H_
#define _CMD_H_

enum COMMANDS {
	#define DEF_CMD(num, name) \
		CMD_ ## name = num,
		
	#include "cmd_def.h"
	
	#undef DEF_CMD
};

#endif /* _CMD_H_ */
