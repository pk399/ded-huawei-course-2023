#ifndef _STACK_H_
#define _STACK_H_

typedef struct _stack Stack;

Stack* StackCtor(unsigned elem_size);
int StackDump(Stack*);
int StackDtor(Stack*);

int StackPush(Stack*, const void* value);
int StackPop(Stack*, void* value);

#endif /* _STACK_H_ */
