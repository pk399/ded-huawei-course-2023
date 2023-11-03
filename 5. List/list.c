#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include <colors.h>

typedef int list_t;
const list_t POISON = -1;
#define LISTTF "%d"
#define DUMPLISTTF "%3d"


struct List {
    list_t* data;
    int* next;
    int* prev;
    
    int head;
    int tail;
    
    int free;
    
    int capacity;
};



List* ListCtor(int capacity);
int ListResizeUp(List* lst, int new_cap);
int ListValidate(const List* lst);
int ListTextDump(const List* lst);
int ListDtor(List* lst);


List* ListCtor(int capacity) {
    assert(capacity > 0);
    
    List* lst = (List*) calloc(1, sizeof(List));
    if (!lst) return NULL;
    
    capacity = 14; // DED example
    
    if ( ListResizeUp(lst, capacity) ) {
        ListDtor(lst);
        return NULL;
    }
    
    // Example
    int data[14] = {-1, 10, 20, 30, 40, 50, 60, -1, 80, -1, 45, -1, -1, -1};
    for (int i=0; i < 14; i++) lst->data[i] = data[i];
    int next[14] = { 0,  2,  3,  4, 10,  6,  8,  9,  0, 11,  5, 12, 13,  0};
    for (int i=0; i < 14; i++) lst->next[i] = next[i];
    int prev[14] = { 0,  0,  1,  2,  3, 10,  5, -1,  6, -1,  4, -1, -1, -1};
    for (int i=0; i < 14; i++) lst->prev[i] = prev[i];
    
    lst->head = 1;
    lst->tail = 8;
    lst->free = 7;
    
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

    if ( !(lst->data && lst->next && lst->prev) ) return -1;

    if ( !(lst->capacity && lst->free) || lst->capacity < 0 || lst->free < 0 ) return -1;

    if ( lst->head < 0 || lst->tail < 0 ) return -1;

    // TODO: validate list sequence
    
    return 0;
}


int ListTextDump(const List* lst) {
    if (ListValidate(lst)) return -1;
    
    printf(MAGENTA("List dump:\n"));
    
    printf("\t     ");
    for (int i = 0; i < lst->capacity; i++) {
        if (i == lst->head)
            printf( GREEN("vvvv"));
        else if (i == lst->tail)
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


int ListDtor(List* lst) {
    if (lst) {
        if (lst->data) free(lst->data);
        if (lst->next) free(lst->next);
        if (lst->prev) free(lst->prev);
        
        *lst = (List) {NULL, NULL, NULL, -1, -1, -1, -1};
        
        free(lst);
    }
    
    return 0;
}


int main() {
    List* lst = ListCtor(16);
    
    ListTextDump(lst);
    
    ListDtor(lst);
}
