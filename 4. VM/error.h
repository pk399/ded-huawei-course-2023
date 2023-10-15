#ifndef _ERROR_H_
#define _ERROR_H_

#include "colors.h"

#define RELAY(code) if (code) return -1

#define FATAL(description) printf(RED("ERROR") ": %s:%d (" __FILE__ "): " description "\n", __func__, __LINE__)
#define WARNING(description) printf(YELLOW("WARNING") ": %s:%d (" __FILE__ "): " description "\n", __func__, __LINE__)

#endif /* _ERROR_H_ */
