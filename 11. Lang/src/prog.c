#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tree.h"
#include "colors.h"

#include "prog.h"

Text* pexport(Program* p) {
    Text* t = tblank();
    assert(t);

    tprintf(t, "v01\n");
    tprintf(t, "%d\n", p->size);
    for (int i = 0; i < p->size; i++) {
        Func f = p->funcs[i];

        tprintf(t, "%d %d ", f.inputs, f.vars_size);

        for (int j = 0; j < f.vars_size; j++) {
            Var v = f.vars[j];

            tprintf(t, "%s %d ", v.name, v.ndims);
            
            for (int k = 0; k < v.ndims; k++) {
                tprintf(t, "%d ", v.dims[k]);
            }
        }

        nexport(t, f.n);
    }

    return t;
}


Program* pimport(Text* t) {
    return NULL; // .....
}


int pdump(Program* p) {
    printf(MAGENTA("Program") "\n");

    for (int i = 0; i < p->size; i++) {
        Func f = p->funcs[i];

        printf("function " YELLOW("%s") " ", f.name);
        for (int j = 0; j < f.inputs; j++) {
            printf("#" BLUE("%s") " ", f.vars[j].name);
        }
        putchar('\n');

        printf("vars: ");
        for (int k = 0; k < f.vars_size; k++) {
            Var v = f.vars[k];

            printf(BLUE("%s"), v.name);
            for (int l = 0; l < v.ndims; l++) {
                printf("[%d]", v.dims[l]);
            }
            putchar(' ');
        }
        putchar('\n');

        ndump(f.n);
    }
}


int pdtor(Program* p) {
    if (p) {
        if (p->funcs) {
            for (int i = 0; i < p->size; i++) {
                Func f = p->funcs[i];

                if (f.name) free(f.name);
                ndtor(f.n);

                for (int j = 0; j < f.vars_size; j++) {
                    Var v = f.vars[j];
                    if (v.dims) free(v.dims);
                    if (v.name) free(v.name);
                }

                free(f.vars);
            }

            free(p->funcs);
        }

        free(p);
    }
}