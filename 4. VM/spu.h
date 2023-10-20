#ifndef _SPU_H_
#define _SPU_H_

#include <stdio.h>

typedef struct _spu SPU;


SPU* SPUCtor();
int SPUDump(SPU*);
int SPUDtor(SPU*);

int SPULoad(SPU*, const char* filename);
int fSPULoad(SPU*, FILE* file);

int SPUStep(SPU*);
int SPUExec(SPU*);

#endif /* _SPU_H_ */
