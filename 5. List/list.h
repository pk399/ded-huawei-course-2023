#ifndef _LIST_H_
#define _LIST_H_

typedef int list_t;
const list_t POISON = -1;
#define LISTTF "%d"
#define DUMPLISTTF "%3d"


struct List {
    list_t* data;
    int* next;
    int* prev;
    
    int free;
    
    int capacity;
};



List* ListCtor(int capacity);
int ListResizeUp(List* lst, int new_cap);
int ListValidate(const List* lst);
int ListTextDump(const List* lst);
int ListGraphDump(const List* lst);
//int ListGraphTemplate(const char* filename, const List* lst, const char* template);
int ListInsert(List* lst, int idx, list_t elem);
int ListRemove(List* lst, int idx);
int ListSwap(List* lst, int idx1, int idx2);
int ListDtor(List* lst);

#endif /* _LIST_H_ */
