#include <stdio.h>

#define DEBUG
#define CANARY
#define HASH

typedef double stack_t;
#define STACKTFORMAT "%lf"
const stack_t STACK_POISON_VALUE = -3;

const size_t STACK_DEFAULT_CAPACITY = 64; // Actually minimum capacity

const size_t GROW_BY = 2; // On push: Multiplies stack capacity by this factor when growing
const size_t SHRINK_WHEN = 8; // On pop: Shrink the stack when size is by this factor smaller than capacity
const size_t SHRINK_BY = 2; // On pop: Shrinks the capacity by this factor
