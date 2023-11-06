#include <stdio.h>
#include <stdlib.h>

#include "list.h"

int main() {
    //ListGraphDump(NULL);
    struct List* lst = ListCtor(16);
    
    //ListTextDump(lst);
    for (int i = 0; i < 100; i++) {
        if (ListInsert(lst, 0, i) ) printf("Insert error'd %d\n", i);
    }
    
    for (int i = 1; i < 99; i++) {
        int idx = i + (rand() / (RAND_MAX / (100 - i)));
        printf("Swapping %d and %d...\n", i, idx);
        ListSwap(lst, i, idx);
    }
    ListGraphDump(lst);
    
    ListDtor(lst);
}
