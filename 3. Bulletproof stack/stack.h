#include <stdio.h>

#include "config.h"


#define STACKCHECK(err, stk) error_t err = StackCheck(stk)
#define IGNOREERR(err, err_val) err = IgnoreError(err, err_val)

#ifdef DEBUG
	#define RETIFERR(err, stk) if (err) {STACKDUMP(stk); return STACK_CHECK_FAILED;}
#else
	#define RETIFERR(err, stk) if (err) {return STACK_CHECK_FAILED;}
#endif

#define STACKCTOR(stk) StackCtor((stk), #stk, __FILE__, __LINE__, __func__)

#ifdef DEBUG
	#define FSTACKDUMP(stk, stream) StackDump((stk), (stream), __FILE__, __LINE__, __func__)
	#define STACKDUMP(stk) StackDump((stk), stdout, __FILE__, __LINE__, __func__)

	#define RED_COLOR "\033[1;31m"
	#define NORMAL_COLOR "\033[1;0m"
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
	const canary_t CANARY_VAL = 0xABCDBEEFCAFEBABE;
	const canary_t CANARY_DTOR_VAL = 0x1337; // Assigned when stack is destroyed
#endif

#ifdef HASH
	typedef unsigned hash_t;
	const hash_t HASH_DTOR_VAL = 0x1337; // Assigned when stack is destroyed
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
    NULL_DATA              = 1 << 0,
    ZERO_CAP               = 1 << 1,
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


error_t IgnoreError(error_t err, ERROR ignore);
size_t StackDataElemsSize(size_t cap);
size_t StackDataSize(size_t cap);
void* ToRealPtr(stack_t* ptr);
stack_t* FromRealPtr(void* ptr);
RESULT StackPop(Stack* stk, stack_t* val);
RESULT StackPush(Stack* stk, stack_t val);
RESULT StackDtor(Stack* stk);
RESULT StackCtor(Stack* stk, const char* var, const char* file, size_t line, const char* func);
RESULT StackRealloc(Stack* stk, size_t ncap);
error_t StackCheck(const Stack* stk);

#ifdef DEBUG
	void StackDump(const Stack* stk, FILE* stream, const char* caller_file, size_t caller_line, const char* caller_func);
#endif

#ifdef CANARY
	canary_t* LeftDataCanaryPtr(stack_t* stk);
	canary_t* RightDataCanaryPtr(stack_t* stk, size_t capacity);
	void WriteStructCanary(Stack* stk, canary_t val);
	void WriteDataCanary(Stack* stk, canary_t val);
	error_t CheckCanary(const Stack* stk);
#endif

#ifdef HASH
	hash_t* DataHashPtr(stack_t* stk);
	error_t CheckHash(Stack* stk);
	void SyncStructHash(Stack* stk);
	void SyncDataHash(Stack* stk);
#endif
