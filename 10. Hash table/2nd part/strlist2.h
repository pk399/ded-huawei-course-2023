#ifndef _STRLIST_H_
#define _STRLIST_H_

const int ELEM_SIZE = 32; // 256 bits for avx
typedef char list_t[ELEM_SIZE];
#define DUMPLISTTF "%s"

typedef struct {
    list_t* data;
    int* next;
    int* prev;
    
    int free;
    
    int capacity;
    int size;
} List;



List* ListCtor(int capacity);
int ListResizeUp(List* lst, int new_cap);
int ListValidate(const List* lst);
int ListTextDump(const List* lst);
int ListInsert(List* lst, int idx, const char* elem);
int ListRemove(List* lst, int idx);
int ListSwap(List* lst, int idx1, int idx2);
int ListDtor(List* lst);

#endif /* _STRLIST_H_ */
