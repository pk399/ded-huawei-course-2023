#ifndef _CODE_H_
#define _CODE_H_

#include <stdio.h>

#include "memory.h"

// File format:
// 1. unsigned - Magic number
// 2. unsigned - Version
// 3. unsigned - Filesize

const unsigned MAGIC   = 69236456;
const unsigned VERSION = 13370000;


int WriteBytecode(FILE*, Memory*);
Memory* ReadBytecode(FILE*);

#endif /* _CODE_H_ */
