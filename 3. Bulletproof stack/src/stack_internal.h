#include "stack_include.h"


#define STACKCHECK(err, stk) error_t err = StackCheck(stk)
#define IGNOREERR(err, err_val) err = IgnoreError(err, err_val)

#ifdef DEBUG
	#define RETIFERR(err, stk) if (err) {STACKDUMP(stk); return STACK_CHECK_FAILED;}
#else
	#define RETIFERR(err, stk) if (err) {return STACK_CHECK_FAILED;}
#endif


#ifdef CANARY
	const canary_t CANARY_VAL = 0xABCDBEEFCAFEBABE;
	const canary_t CANARY_DTOR_VAL = 0x1337; // Assigned when stack is destroyed
#endif

#ifdef HASH
	const hash_t HASH_DTOR_VAL = 0x1337; // Assigned when stack is destroyed
#endif


error_t IgnoreError(error_t err, ERROR ignore);
size_t StackDataElemsSize(size_t cap);
size_t StackDataSize(size_t cap);
void* ToRealPtr(stack_t* ptr);
stack_t* FromRealPtr(void* ptr);

#ifdef CANARY
	canary_t* LeftDataCanaryPtr(stack_t* stk);
	canary_t* RightDataCanaryPtr(stack_t* stk, size_t capacity);
#endif

#ifdef HASH
	hash_t* DataHashPtr(stack_t* stk);
#endif
