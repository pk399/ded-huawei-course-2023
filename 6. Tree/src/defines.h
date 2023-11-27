#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <stdio.h>

const unsigned BUF_SZ = 512; // Default size for temp buffers


#define L(n) (n)->left
#define R(n) (n)->right

#define SHPRINTF(buf, sz, ...) if (*(sz) != 0) {int tmp = snprintf(*(buf), *(sz), __VA_ARGS__); *(buf) += tmp; if (tmp < *(sz) - 1) {*(sz) -= tmp;} else {*(sz) = 0;}}

#endif /* _DEFINES_H_ */