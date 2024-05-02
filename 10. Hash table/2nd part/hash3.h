#ifndef _HASH_H_
#define _HASH_H_

#include <stdint.h>

#include "strlist2.h"

const int DEFAULT_LIST_CAP = 4;

typedef uint64_t (*hashf_t)(list_t s);

typedef struct {
	List** data;

	int capacity;

	hashf_t func;
} HT;

HT* HTCtor(int capacity, hashf_t func);
int HTValidate(const HT* ht);
int HTDtor(HT* ht);
int HTDump(const HT* ht); 
int HTInsert(HT* ht, const char* s);
int HTLookUp(const HT* ht, const char* s);


#endif /* _HASH_H_ */
