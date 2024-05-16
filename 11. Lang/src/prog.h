#ifndef _PROG_H_
#define _PROG_H_

#include "tree.h"

typedef struct {
    int ndims;
    int* dims;
    char* name;
} Var;

typedef struct {
    char* name;

    int inputs;
    int vars_size;
    Var* vars;

    Node* n;
} Func;

typedef struct {
    int size;
    Func* funcs;
} Program;

Text* pexport(Program* p);
Program* pimport(Text* t);

int pdump(Program* p);
int pdtor(Program* p);

#endif /* _PROG_H_ */