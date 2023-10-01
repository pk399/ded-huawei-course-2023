#include <cstddef>

#include "stack.h"

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
	typedef long long unsigned hash_t;
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
