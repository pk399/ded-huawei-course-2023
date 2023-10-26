#include <stdio.h>

#include "config.h"


#define STACKCTOR(stk) StackCtor((stk), #stk, __FILE__, __LINE__, __func__)

#ifdef DEBUG
	#define FSTACKDUMP(stk, stream) StackDump((stk), (stream), __FILE__, __LINE__, __func__)
	#define STACKDUMP(stk) StackDump((stk), stdout, __FILE__, __LINE__, __func__)
#endif


typedef unsigned error_t;

#ifdef DEBUG
	struct DebugData {
		const char* var;
		const char* func;
		size_t line;
		const char* file;
	};
#endif

#ifdef CANARY
	typedef long long unsigned canary_t;
#endif

#ifdef HASH
	typedef unsigned hash_t;
#endif


struct Stack {
    #ifdef CANARY
        canary_t left_canary;
    #endif

    stack_t* data;
    size_t capacity;
    size_t size;
    
    #ifdef HASH
        hash_t hash;
        //hash_t data_hash
    #endif

    #ifdef DEBUG
    	DebugData debug;
    #endif

    #ifdef CANARY
        canary_t right_canary;
    #endif
};


// For stack state
enum ERROR {
    NULL_DATA              = 1 << 0, // 
    ZERO_CAP               = 1 << 1, // Error if cap != 0, data = NULL
    SIZE_GT_CAP            = 1 << 2,
    
    #ifdef CANARY
		LEFT_STRUCT_CANARY     = 1 << 3,
		RIGHT_STRUCT_CANARY    = 1 << 4,
		LEFT_DATA_CANARY       = 1 << 5,
		RIGHT_DATA_CANARY      = 1 << 6,
    #endif
    
    #ifdef HASH
		STRUCT_HASH            = 1 << 7,
		DATA_HASH              = 1 << 8,
    #endif
    
    #ifdef DEBUG
    	NO_DEBUG_DATA          = 1 << 9,
    #endif
};

// Returned by stack functions
enum RESULT {
	SUCCESS = 0,
	STACK_CHECK_FAILED = -1,
	REALLOC_FAILED = -2,
	NO_ELEMS = -3,
};


RESULT StackPop(Stack* stk, stack_t* val);
RESULT StackPush(Stack* stk, stack_t val);
RESULT StackDtor(Stack* stk);
RESULT StackCtor(Stack* stk, const char* var, const char* file, size_t line, const char* func);
RESULT StackRealloc(Stack* stk, size_t ncap);
error_t StackCheck(const Stack* stk);


// Into internal.h
#ifdef DEBUG
	void StackDump(const Stack* stk, FILE* stream, const char* caller_file, size_t caller_line, const char* caller_func);
#endif

#ifdef CANARY
	void WriteCanary(Stack* stk, canary_t val);
	error_t CheckCanary(const Stack* stk);
#endif

#ifdef HASH
	error_t CheckHash(Stack* stk);
	void SyncHash(Stack* stk);
#endif
