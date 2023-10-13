#ifndef _VM_H_
#define _VM_H_

#include <stdio.h>

typedef struct _vm VM;


VM* VMCtor(unsigned mem_size);
int VMDump(VM*);
int VMDtor(VM*);

int VMLoad(VM*, const char* filename);
int fVMLoad(VM*, FILE* file);

int VMStep(VM*);
int VMExec(VM*);

#endif /* _VM_H_ */
