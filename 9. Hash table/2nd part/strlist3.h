#ifndef _STRLIST_H_
#define _STRLIST_H_

const int ELEM_SIZE = 32; // 256 bits for avx
typedef char list_t[ELEM_SIZE];
#define DUMPLISTTF "%s"

typedef struct {
    list_t* data;
    
    int capacity;
    int size; // a.k.a. free pointer
} List;



List* ListCtor(int capacity);
int ListResizeUp(List* lst, int new_cap);
int ListTextDump(const List* lst);
int ListInsert(List* lst, const char* elem);
//int ListRemove(List* lst, int idx); todo?
int ListDtor(List* lst);

#endif /* _STRLIST_H_ */
