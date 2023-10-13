#ifndef _FUNC_STACK_H_
#define _FUNC_STACK_H_

typedef struct _stack Stack;

typedef char stack_t;

Stack* StackCtor();
int StackDump(Stack*);
int StackDtor(Stack*);

int StackPush(Stack*, stack_t value);
int StackPop(Stack*, stack_t* value);

#endif /* _FUNC_STACK_H_ */
