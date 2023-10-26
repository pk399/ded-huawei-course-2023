#include "stack_internal.h"


#ifdef HASH
hash_t* DataHashPtr(stack_t* ptr) {
	return ( (hash_t*) ptr ) - 1;
}


hash_t DJBHash(void* ptr, size_t size) {
	hash_t h = 5381;
	
	for (size_t i = 0; i < size; i++) {
		unsigned char c = ( (char*) ptr )[i];
		h = h * 33 + c;
	}
	
	return h;
}


error_t CheckHash(Stack* stk) {
	error_t err = 0;

	hash_t struct_hash = stk->hash;
	stk->hash = 0;
	if (DJBHash(stk, sizeof(Stack)) != struct_hash)
		err |= STRUCT_HASH;
	stk->hash = struct_hash;
	
	if (stk->data) {
		hash_t data_hash = DJBHash(stk->data, StackDataElemsSize(stk->capacity));
		
		if (data_hash != *DataHashPtr(stk->data))
			err |= DATA_HASH;
	}
	
	return err;
}


void SyncHash(Stack* stk) {
	stk->hash = 0;
	stk->hash = DJBHash(stk, sizeof(Stack));

	if (stk->data)
		*DataHashPtr(stk->data) = DJBHash(stk->data, StackDataElemsSize(stk->capacity));
}
#endif // HASH
