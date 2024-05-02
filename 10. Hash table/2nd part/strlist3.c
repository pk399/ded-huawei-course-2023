#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <colors.h>

#include "strlist3.h"


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
    if (!lst->data) {
        lst->data = (list_t*) aligned_alloc(32, new_cap*sizeof(list_t));
    } else {
        lst->data = (list_t*) realloc(lst->data, new_cap*sizeof(list_t));
    }
    
    if ( !lst->data ) {
        return -1;
    }
    
    for (int i = lst->capacity; i < new_cap; i++) {
        memset(lst->data[i], 0, ELEM_SIZE);
    }
    
    lst->capacity = new_cap;
    
    return 0;
}


int ListTextDump(const List* lst) {
    printf(MAGENTA("List dump (size = ") YELLOW("%d") MAGENTA("):\n"), lst->size);
    
    printf("\tData:");
    for (int i = 0; i < lst->capacity; i++) {
        printf(" " DUMPLISTTF, lst->data[i]);
    }
    printf("\n");
    
    return 0;
}


int ListInsert(List* lst, const char* elem) {
    if (lst->size == lst->capacity)
        if (ListResizeUp(lst, lst->capacity * 2)) return -1;
    
    int new_node = lst->size;
    // Insert
    strncpy(lst->data[new_node], elem, ELEM_SIZE);
    
    lst->size++;
    
    return 0;
}


/*
int ListRemove(List* lst, int idx) {
    memset(lst->data[idx], 0, ELEM_SIZE);
    lst->prev[lst->next[idx]] = lst->prev[idx];
    lst->next[lst->prev[idx]] = lst->next[idx];
    
    lst->prev[idx] = -1;
    lst->next[idx] = lst->free;
    lst->free = idx;
    
    lst->size--;
    
    return 0;
}
*/


int ListDtor(List* lst) {
    if (lst) {
        if (lst->data) free(lst->data);
        
        *lst = (List) {NULL, -1, -1};
        
        free(lst);
    }
    
    return 0;
}
