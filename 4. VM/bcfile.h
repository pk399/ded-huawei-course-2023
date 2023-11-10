#ifndef _BCFILE_H_
#define _BCFILE_H_

#include <stdio.h>

// File format:
// 1. unsigned - Magic number
// 2. unsigned - Version
// 3. unsigned - Filesize

const unsigned MAGIC   = 69236456;
const unsigned VERSION = 13370005;


int WriteBytecode(FILE*, char*, unsigned sz);
int LoadBytecode(char*, unsigned* sz, FILE*);

#endif /* _BCFILE_H_ */
