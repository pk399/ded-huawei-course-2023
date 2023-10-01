#include <assert.h>

#include "stack.h"


canary_t* LeftDataCanaryPtr(stack_t* ptr) {
	#ifdef HASH // Hash is left from data, then canary
		return ( (canary_t*) DataHashPtr(ptr) ) - 1;
	#else
		return ( (canary_t*) ptr ) - 1;
	#endif
}


canary_t* RightDataCanaryPtr(stack_t* ptr, size_t capacity) {
	return (canary_t*) ( (char*) ptr + StackDataElemsSize(capacity));
}


void WriteStructCanary(Stack* stk, canary_t val) {
	stk->left_canary = val;
	stk->right_canary = val;
}

void WriteDataCanary(Stack* stk, canary_t val) {
	*LeftDataCanaryPtr(stk->data) = val;
	*RightDataCanaryPtr(stk->data, stk->capacity) = val;
}


error_t CheckCanary(const Stack* stk) {
    assert(stk);

    error_t err = 0;

    err |= LEFT_STRUCT_CANARY * (stk->left_canary != CANARY_VAL);
    err |= RIGHT_STRUCT_CANARY * (stk->right_canary != CANARY_VAL);
    
    if (!stk->data) // Don't check data canaries, if pointer is NULL
        return err;
    
    err |= LEFT_DATA_CANARY * (*LeftDataCanaryPtr(stk->data) != CANARY_VAL);
    err |= RIGHT_DATA_CANARY * (*RightDataCanaryPtr(stk->data, stk->capacity) != CANARY_VAL);

    return err;
}
