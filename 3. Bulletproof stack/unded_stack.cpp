#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
// make include.h

#define DEBUG
#define CANARY
// #define HASH

#ifdef CANARY
    typedef long long unsigned canary_t;
    const canary_t CANARY_VAL = 0xABCDBEEFCAFEBABE;
    const canary_t CANARY_DTOR_VAL = 0x1337;
#endif

#ifdef HASH
    typedef long long unsigned hash_t;
#endif

#define STACKTFORMAT "%d"
typedef int stack_t;
const stack_t STACK_POISON = -3;

const size_t STACK_DEF_SIZE = 5;

const size_t GROW_MULT = 2;
const size_t SHRINK_MULT = 3;

typedef unsigned error_t;


struct Stack {
    #ifdef CANARY
        canary_t left_canary;
    #endif
    
    #ifdef HASH
        hash_t hash;
    #endif

    stack_t* data;
    size_t capacity;
    size_t size;

    #ifdef DEBUG
    	// put into debug struct
        const char* var;
        const char* func;
        size_t line;
        const char* file;
    #endif

    #ifdef CANARY
        canary_t right_canary;
    #endif
};


enum ERROR {
    NULLDATA    = 1 << 0,
    ZEROCAP     = 1 << 1,
    SIZEGTCAP   = 1 << 2,
    // L - left, R - right
    // S - struct, D - data array
    LSCANARY    = 1 << 3,
    RSCANARY    = 1 << 4,
    LDCANARY    = 1 << 5,
    RDCANARY    = 1 << 6,
    SHASH       = 1 << 7,
    DHASH       = 1 << 8,
    NODBGDATA   = 1 << 9,
};


#ifdef CANARY
error_t CheckCanary(const Stack* stk) {
    assert(stk);

    error_t err = 0;

    err |= LSCANARY * (stk->left_canary != CANARY_VAL);
    err |= RSCANARY * (stk->right_canary != CANARY_VAL);
    
    if (!stk->data)
        return err;

    err |= LDCANARY * (( (canary_t*)stk->data )[-1] != CANARY_VAL);

    size_t size = sizeof(stack_t) * stk->capacity;
    // 8 -> sizeof(canary_t), turn into a function
    if (size % 8)
        size += 8 - size % 8;
    err |= RDCANARY * ( *(canary_t*) ( (char*)stk->data + size ) != CANARY_VAL);

    return err;
}
#endif

// StackVal -> StackValidate , StackCheck
error_t StackVal(const Stack* stk) {
    assert(stk);

    error_t err = 0;

    if (!stk->data)
        err |= NULLDATA;

    if (!stk->capacity)
        err |= ZEROCAP;

    if (stk->size > stk->capacity)
        err |= SIZEGTCAP; // Turn into a macro

    #ifdef DEBUG
    if (!stk->var
        || !stk->func
        || !stk->file
        || !stk->line)
 		err |= NODBGDATA;
    #endif

    #ifdef CANARY
    err |= CheckCanary(stk);
    #endif

    // TODO: hash
    return err;
}


#define STACKDUMP(stk) StackDump( (stk), __FILE__, __LINE__, __func__)

// call file, line, func differently
void StackDump(const Stack* stk, const char* file, size_t line, const char* func) {
    assert(stk);

    error_t err = StackVal(stk);

	// Print into a file or stdout

    printf("Stack [%p] ", stk);

    if (err & NODBGDATA) 
        printf("\033[1;31m" "no debug data inside Stack" "\033[1;0m\n"); // colors into define
    else
        printf("\"%s\" from %s(%zd) %s()\n",
               stk->var, stk->file, stk->line, stk->func);
    

    #define PAD "                       "

    printf(PAD "called from %s(%zd) %s()\n\n", file, line, func);
    
    if (err & ~NODBGDATA) {
        printf("Errors:\n");

        if (err & NULLDATA)
            printf("- Data pointer is NULL\n"); // Make a macro

        if (err & ZEROCAP)
            printf("- Capacity is zero\n");

        if (err & SIZEGTCAP)
            printf("- Size is greater than capacity\n");

        if (err & RSCANARY)
            printf("- Right struct canary is corrupted\n");

        if (err & LSCANARY)
            printf("- Left struct canary is corrupted\n");

        if (err & RDCANARY)
            printf("- Right data canary is corrupted\n");

        if (err & LDCANARY)
            printf("- Left data canary is corrupted\n");

        if (err & SHASH)
            printf("- Struct hash mismatch\n");

        if (err & DHASH)
            printf("- Data hash mistmatch\n");

        putchar('\n');
    }

    printf("{\n");

    #define TAB "\t"

    printf(TAB "size=%zd\n", stk->size);
    printf(TAB "capacity=%zd\n", stk->capacity);
    printf(TAB "data[%p]\n", stk->data);

    if (stk->data) {
        printf(TAB "{\n");

        for (size_t i = 0; i < stk->capacity; i++) {
            printf(TAB "%s[%zd]=" STACKTFORMAT,
                   (i < stk->size) ? "*" : " " /*extract into a different action*/, i, stk->data[i]);

            if (stk->data[i] == STACK_POISON)
                printf("(POISON)\n");
            else
                putchar('\n');
        }

        printf(TAB "}\n");
    }

    printf("}\n");

    #undef PAD
    #undef TAB
}


// ncap -> newcap
int StackRealloc(Stack* stk, size_t ncap) {
    assert(stk);
    assert(ncap);

    error_t err = StackVal(stk);
    err &= ~NULLDATA; // func IgnoreFlag
    err &= ~ZEROCAP;
    if (err) { 
        STACKDUMP(stk);
        return -1;
    }

    #ifdef CANARY
    if (stk->data)
        stk->data = (stack_t*) ((char*)stk->data - sizeof(canary_t));
    #endif

    size_t size = ncap * sizeof(stack_t);
    #ifdef CANARY
    if (size % 8)
        size += 8 - size % 8;
    size += 2 * sizeof(canary_t);
    #endif

    void* res = realloc(stk->data, size); // alloc_size

    if (!res) {
        return -1; // enum for function errors
    }
    
    #ifdef CANARY
    res = (char*)res + sizeof(canary_t);
    #endif
    stk->data = (stack_t*) res;

    if (ncap > stk->capacity)
        for (size_t i=stk->capacity; i < ncap; i++)
            stk->data[i] = STACK_POISON;

    stk->capacity = ncap;

    #ifdef CANARY
    ( (canary_t*)stk->data )[-1] = CANARY_VAL; // extract FillLeftCanary, RightCanary
    *(canary_t*) ( (char*)stk->data + size - 2 * sizeof(canary_t) ) = CANARY_VAL;
    #endif

    return 0;
}


#define STACKCTOR(stk) StackCtor((stk), #stk, __FILE__, __LINE__, __func__)

int StackCtor(Stack* stk, const char* var, const char* file, size_t line, const char* func) {
    assert(stk);

    #ifdef DEBUG
    stk->var = var; // check if NULL
    stk->file = file;
    stk->line = line;
    stk->func = func;
    #endif

    #ifdef CANARY
    stk->left_canary = CANARY_VAL;
    stk->right_canary = CANARY_VAL;
    #endif

    stk->size = 0;
    StackRealloc(stk, STACK_DEF_SIZE);

    error_t err = StackVal(stk);
    if (err) {
        STACKDUMP(stk);
        return -1;
    }

    return 0;
}


int StackDtor(Stack* stk) {
    assert(stk);

    error_t err = StackVal(stk);
    if (err) {
        STACKDUMP(stk);
        return -1;
    }

    #ifdef CANARY
    stk->data = (stack_t*) ((canary_t*)stk->data - 1);
    #endif

    free(stk->data);

    stk->data = 0;

    stk->size = 0;
    stk->capacity = 0;
	
	#ifdef CANARY
    stk->left_canary = CANARY_DTOR_VAL;
    stk->right_canary = CANARY_DTOR_VAL;
    #endif

    return 0;
}
    

int StackPush(Stack* stk, stack_t val) {
    assert(stk);

    error_t err = StackVal(stk);
    if (err) {
        STACKDUMP(stk);
        return -1;
    }


    if (stk->size == stk->capacity) {
        int res = StackRealloc(stk, stk->capacity*GROW_MULT);
        if (res)
            return -printf("Error reallocating!\n");
    }

    stk->data[stk->size++] = val;

    return 0;
}


int StackPop(Stack* stk, stack_t* val) {
    assert(stk);
    assert(val);

    error_t err = StackVal(stk);
    if (err) {
        STACKDUMP(stk);
        return -1;
    }

    if (stk->size == 0) {
        return -printf("No elements inside the stack!\n");
    }

    *val = stk->data[stk->size - 1];
    stk->data[--stk->size] = STACK_POISON;

    if (stk->size <= stk->capacity/SHRINK_MULT) {
        int res = StackRealloc(stk, stk->capacity/SHRINK_MULT);
        if (res)
            return -printf("Error shrinking!\n");
    }

    return 0;
}


int main() {
    Stack stk = {};
    STACKCTOR(&stk);

    StackPush(&stk, 10);
    ((char*) stk.data)[-1] = 'g';
    StackPush(&stk, 20);
    StackPush(&stk, 30);

    stack_t val = 0;
    StackPop(&stk, &val);
    printf(STACKTFORMAT "\n", val);

    STACKDUMP(&stk);

    StackDtor(&stk);
}
