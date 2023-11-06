#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include <colors.h>

#include "list.h"


List* ListCtor(int capacity) {
    assert(capacity > 0);
    
    List* lst = (List*) calloc(1, sizeof(List));
    if (!lst) return NULL;
    
    if ( ListResizeUp(lst, capacity) ) {
        ListDtor(lst);
        return NULL;
    }
    
    return lst;
}


int ListResizeUp(List* lst, int new_cap) {
    assert(lst);
    assert(new_cap > lst->capacity);
    assert(lst->capacity >= 0);
    
    lst->data = (list_t*) realloc(lst->data, new_cap*sizeof(list_t));
    lst->next = (int*) realloc(lst->next, new_cap*sizeof(int));
    lst->prev = (int*) realloc(lst->prev, new_cap*sizeof(int));
    
    if ( !(lst->data && lst->next && lst->prev) ) {
        return -1;
    }
    
    for (int i = lst->capacity; i < new_cap; i++) {
        lst->data[i] = POISON;
    }
    
    for (int i = lst->capacity; i < new_cap; i++) {
        lst->next[i] = lst->free;
        lst->free = i;
    }
    
    for (int i = lst->capacity; i < new_cap; i++) {
        lst->prev[i] = -1;
    }
    
    lst->capacity = new_cap;
    
    return 0;
}


int ListValidate(const List* lst) {
    if (!lst) return -1;

    if ( !lst->data ) return -1;

    if ( !(lst->capacity) || lst->capacity < 0 || lst->free < 0 ) return -1;

    // TODO: validate list sequence
    
    return 0;
}


int ListTextDump(const List* lst) {
    if (ListValidate(lst)) return -1;
    
    printf(MAGENTA("List dump:\n"));
    
    printf("\t     ");
    for (int i = 0; i < lst->capacity; i++) {
        if (i == lst->next[0])
            printf( GREEN("vvvv"));
        else if (i == lst->prev[0])
            printf(YELLOW("vvvv"));
        else if (i == lst->free)
            printf(   RED("vvvv"));
        else
            printf(       "    " );
    }
    printf("\n");
    
    printf("\t     ");
    for (int i = 0; i < lst->capacity; i++) {
        printf(BLUE(" %3d"), i);
    }
    printf("\n");
    
    printf("\tData:");
    for (int i = 0; i < lst->capacity; i++) {
        printf(" " DUMPLISTTF, lst->data[i]);
    }
    printf("\n");
    
    printf("\tNext:");
    for (int i = 0; i < lst->capacity; i++) {
        printf(" %3d", lst->next[i]);
    }
    printf("\n");
    
    printf("\tPrev:");
    for (int i = 0; i < lst->capacity; i++) {
        printf(" %3d", lst->prev[i]);
    }
    printf("\n");
    
    return 0;
}


int ListGraphDump(const List* lst) {
    // TODO: this -> int GetTime(char* buf)
    time_t t = time(NULL);
    assert(t != -1);
    struct tm* lt = localtime(&t);
    
    const unsigned MX_FILENAME_LEN = 32;
    char timestr[MX_FILENAME_LEN] = {};
    strftime(timestr, MX_FILENAME_LEN, "%Y-%m-%d %H:%M:%S", lt);
    // ------------------------------------
    
    //ListGraphTemplate(fn, lst, "template1.dot"); Doing it manually
    
    char strbuf[MX_FILENAME_LEN + 100] = "";
    
    sprintf(strbuf, "dot -Tpng -o \"Dump %s.png\"", timestr);
    
    FILE* f = popen(strbuf, "w");
    //FILE* f = fopen("test.dot", "wt");
    if (!f) return -1;
    
    fprintf(f, "digraph {\n rankdir=LR;\n subgraph cluster {\nstyle=filled;\n color=lightgrey;\n label=\"List ultra mega\"\n");
    
    for (int i = 0; i < lst->capacity; i++) {
        fprintf(f, " n%d[shape=\"Mrecord\", label=\"%d | " LISTTF " | <n> next(%d) | <p> prev(%d)\" ", i, i, lst->data[i], lst->next[i], lst->prev[i]);
        if (i == 0) fprintf(f, "color=\"purple\"");
        else if (lst->prev[i] == -1) fprintf(f, "color=\"lime\"");
        fprintf(f, "]\n");
    }
    
    
    fprintf(f, "} n0");
    for (int i = 1; i < lst->capacity; i++) {
        fprintf(f, " -> n%d", i);
    }
    fprintf(f, "[weight=\"69420236\", color=\"#00000000\"];\n");
    
    for (int i = 0; i < lst->capacity; i++) {
        if (lst->prev[i] >= 0)
            fprintf(f, "n%d: <p> -> n%d [color=\"red\"];\n", i, lst->prev[i]);
        if (lst->next[i] >= 0)
            fprintf(f, "n%d: <n> -> n%d [color=\"blue\"];\n", i, lst->next[i]);
    }
    
    fprintf(f, "free -> n%d [color=\"green\"]\n", lst->free);
    
    fprintf(f, "}\n");
    
    pclose(f);
    return 0;
}


int ListInsert(List* lst, int idx, list_t elem) {
    assert(lst);
    assert(idx >= 0);
    if (ListValidate(lst)) return -1;
    
    if (lst->free == 0)
        if (ListResizeUp(lst, lst->capacity * 2)) return -1;
    
    int new_node = lst->free;
    lst->free = lst->next[new_node];
    
    lst->prev[lst->next[idx]] = new_node;
    lst->next[new_node] = lst->next[idx];
    lst->prev[new_node] = idx;
    lst->next[idx] = new_node;
    lst->data[new_node] = elem;
    
    //lst->size++; No size yet
    
    return 0;
}


int ListRemove(List* lst, int idx) {
    assert(lst);
    assert(idx > 0);
    if (ListValidate(lst)) return -1;
    
    lst->data[idx] = POISON;
    lst->prev[lst->next[idx]] = lst->prev[idx];
    lst->next[lst->prev[idx]] = lst->next[idx];
    
    lst->prev[idx] = -1;
    lst->next[idx] = lst->free;
    lst->free = idx;
    
    //lst->size--; No size yet
    
    return 0;
}


int ListSwap(List* lst, int idx1, int idx2) {
    assert(lst);
    
    int temp = lst->next[idx1];
    lst->next[idx1] = lst->next[idx2];
    lst->next[idx2] = temp;
    
    temp = lst->prev[idx1];
    lst->prev[idx1] = lst->prev[idx2];
    lst->prev[idx2] = temp;
    
    return 0;
}


int ListDtor(List* lst) {
    if (lst) {
        if (lst->data) free(lst->data);
        if (lst->next) free(lst->next);
        if (lst->prev) free(lst->prev);
        
        *lst = (List) {NULL, NULL, NULL, -1, -1};
        
        free(lst);
    }
    
    return 0;
}
