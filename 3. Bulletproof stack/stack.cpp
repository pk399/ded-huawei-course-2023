#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "stack_internal.h"


error_t IgnoreError(error_t err, ERROR ignore) {
	return err & ~ignore;
}


error_t StackCheck(const Stack* stk) {
    assert(stk);

    error_t err = 0;
	
	
	#define ERRIF(cond, err_val) if (cond) err |= err_val
	
    ERRIF(!stk->data, NULL_DATA);
    ERRIF(!stk->capacity, ZERO_CAP);
	ERRIF(stk->size > stk->capacity, SIZE_GT_CAP);

    #ifdef DEBUG
		ERRIF(!stk->debug.var
		      || !stk->debug.func
		      || !stk->debug.file
		      || !stk->debug.line,
	 		  NO_DEBUG_DATA);
    #endif
    
    #undef ERRIF
	
	
    #ifdef CANARY
    	err |= CheckCanary(stk);
    #endif
    
	#ifdef HASH
		err |= CheckHash( (Stack*) stk);
	#endif
    
    return err;
}


size_t StackDataElemsSize(size_t cap) {
	size_t size = cap * sizeof(stack_t);
	
	#ifdef CANARY // Allign to canary size
		if (size % sizeof(canary_t))
			size += sizeof(canary_t) - size % sizeof(canary_t);
	#endif
	
	return size;
}


size_t StackDataSize(size_t cap) {
	size_t size = StackDataElemsSize(cap);
	
	#ifdef CANARY
		size += 2 * sizeof(canary_t);
	#endif
	
	#ifdef HASH
		size += sizeof(hash_t);
	#endif
	
	return size;
}


void* ToRealPtr(stack_t* ptr) {
	#ifdef HASH
		#ifndef CANARY
			ptr = (stack_t*) DataHashPtr(ptr);
		#endif
	#endif
	
	#ifdef CANARY
		ptr = (stack_t*) LeftDataCanaryPtr(ptr);
	#endif
	
	return (void*) ptr;
}


stack_t* FromRealPtr(void* ptr) {
	#ifdef CANARY
		ptr = (void*) ( ((canary_t*) ptr) + 1 );
	#endif
	
	#ifdef HASH
		ptr = (void*) ( ((hash_t*) ptr) + 1 );
	#endif
	
	return (stack_t*) ptr;
}


RESULT StackRealloc(Stack* stk, size_t new_cap) {
    assert(stk);
    assert(new_cap);
    
	STACKCHECK(err, stk);
    IGNOREERR(err, NULL_DATA);
    IGNOREERR(err, ZERO_CAP);
    RETIFERR(err, stk);

	void* ptr = 0;
    if (stk->data)
    	ptr = ToRealPtr(stk->data);

    void* res = realloc(ptr, StackDataSize(new_cap));

    if (!res)
        return REALLOC_FAILED;
 
  	stk->data = FromRealPtr(res);
 	
    if (new_cap > stk->capacity)
        for (size_t i = stk->capacity; i < new_cap; i++)
            stk->data[i] = STACK_POISON_VALUE;

    stk->capacity = new_cap;

    #ifdef HASH
		SyncHash(stk);
	#endif

    #ifdef CANARY
    	WriteCanary(stk, CANARY_VAL);
    #endif

    return SUCCESS;
}


RESULT StackCtor(Stack* stk, const char* var, const char* file, size_t line, const char* func) {
    assert(stk);

    #ifdef DEBUG
    	assert(var && file && line && func);
    
		stk->debug.var  = var;
		stk->debug.file = file;
		stk->debug.line = line;
		stk->debug.func = func;
    #endif

    #ifdef CANARY
		WriteCanary(stk, CANARY_VAL);
    #endif

    stk->size = 0;
    stk->capacity = 0;
    stk->data = NULL;
    
    #ifdef HASH
	    SyncHash(stk);
	#endif

	RESULT res = StackRealloc(stk, STACK_DEFAULT_CAPACITY); 
	if (res) {
		StackDtor(stk);
		return res;
	}

    STACKCHECK(err, stk);
    RETIFERR(err, stk);

    return SUCCESS;
}


RESULT StackDtor(Stack* stk) {
    assert(stk);

    STACKCHECK(err, stk);
    RETIFERR(err, stk);
	
	if (stk->data) {
		void* ptr = ToRealPtr(stk->data);
		free(ptr);
		stk->data = 0;
	}

    stk->size = 0;
    stk->capacity = 0;
	
	#ifdef CANARY
		WriteCanary(stk, CANARY_DTOR_VAL);
    #endif
    
    #ifdef HASH
    	stk->hash = HASH_DTOR_VAL;
    #endif

    return SUCCESS;
}
    

RESULT StackPush(Stack* stk, stack_t val) {
    assert(stk);

    STACKCHECK(err, stk);
    RETIFERR(err, stk);

    if (stk->size == stk->capacity) {
        RESULT res = StackRealloc(stk, stk->capacity * GROW_BY);
        if (res)
            return res;
    }

    stk->data[stk->size++] = val;
    
    #ifdef HASH
		SyncHash(stk);
	#endif

    return SUCCESS;
}


RESULT StackPop(Stack* stk, stack_t* val) {
    assert(stk);
    assert(val);

    STACKCHECK(err, stk);
    RETIFERR(err, stk);

    if (stk->size == 0) {
        return NO_ELEMS;
    }

    *val = stk->data[stk->size - 1];
    stk->data[--stk->size] = STACK_POISON_VALUE;
    
    #ifdef HASH
		SyncHash(stk);
	#endif

    if (stk->size <= stk->capacity / SHRINK_WHEN) {
        RESULT res = StackRealloc(stk, stk->capacity / SHRINK_BY);
        if (res)
            return res;
    }

    return SUCCESS;
}
