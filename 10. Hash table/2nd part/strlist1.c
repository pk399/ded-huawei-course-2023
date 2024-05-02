#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <colors.h>

#include "strlist.h"


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
#ifdef DEBUG
    assert(lst);
    assert(new_cap > lst->capacity);
    assert(lst->capacity >= 0);
#endif /* DEBUG */    
    
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
#ifdef DEBUG
    if (ListValidate(lst)) return -1;
#endif /* DEBUG */

    printf(MAGENTA("List dump (size = ") YELLOW("%d") MAGENTA("):\n"), lst->size);
    
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

int ListInsert(List* lst, int idx, list_t elem) {
#ifdef DEBUG
    assert(lst);
    assert(idx >= 0);
    if (ListValidate(lst)) return -1;
#endif /* DEBUG */

    if (lst->free == 0)
        if (ListResizeUp(lst, lst->capacity * 2)) return -1;
    
    int new_node = lst->free;
    lst->free = lst->next[new_node];
    
    lst->prev[lst->next[idx]] = new_node;
    lst->next[new_node] = lst->next[idx];
    lst->prev[new_node] = idx;
    lst->next[idx] = new_node;

    // clone the string
    lst->data[new_node] = strdup(elem);
    
    lst->size++;
    
    return 0;
}


int ListRemove(List* lst, int idx) {
#ifdef DEBUG
    assert(lst);
    assert(idx > 0);
    if (ListValidate(lst)) return -1;
    if (lst->prev[idx] == -1) return -1;
#endif /* DEBUG */

    // free the string
    free(lst->data[idx]);

    lst->data[idx] = POISON;
    lst->prev[lst->next[idx]] = lst->prev[idx];
    lst->next[lst->prev[idx]] = lst->next[idx];
    
    lst->prev[idx] = -1;
    lst->next[idx] = lst->free;
    lst->free = idx;
    
    lst->size--;
    
    return 0;
}


int ListSwap(List* lst, int idx1, int idx2) {
#ifdef DEBUG
    assert(lst);
#endif /* DEBUG */
    
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
	if (ListValidate(lst) != -1) {
		for (int node = lst->next[0]; node != 0; node = lst->next[node]) {
			free(lst->data[node]);
		}
	}
	
        if (lst->data) free(lst->data);
        if (lst->next) free(lst->next);
        if (lst->prev) free(lst->prev);
        
        *lst = (List) {NULL, NULL, NULL, -1, -1};
        
        free(lst);
    }
    
    return 0;
}
