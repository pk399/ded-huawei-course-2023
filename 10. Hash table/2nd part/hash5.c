#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <immintrin.h>

#include "strlist3.h"
#include "colors.h"

#include "hash4.h"


HT* HTCtor(int capacity, hashf_t func) {
	assert(capacity > 1);
	assert(func);

	HT* ht = (HT*) calloc(1, sizeof(HT));
	if (ht == NULL) {
		return NULL;
	}

	ht->func = func;
	ht->capacity = capacity;

	ht->data = (List**) calloc(capacity, sizeof(List*));
	if (ht->data == NULL) {
		free(ht);
		return NULL;
	}

	for (int i = 0; i < capacity; i++) {
		ht->data[i] = ListCtor(DEFAULT_LIST_CAP);
		if (ht->data[i] == NULL) {
			for (int j = i-1; j >= 0; j--) {
				free(ht->data[j]);
			}
			free(ht->data);
			free(ht);
			return NULL;
		}
	}

	return ht;
}


int HTValidate(const HT* ht) {
	if (!ht) return -1;
	if (ht->capacity <= 0) return -1;
	if (!ht->func) return -1;
	if (!ht->data) return -1;

	//ListValidate)

	return 0;
}


int HTDtor(HT* ht) {
	if (HTValidate(ht)) return -1;
	for (int i = 0; i < ht->capacity; i++) {
		ListDtor(ht->data[i]);
	}

	free(ht->data);
	free(ht);

	return 0;
}


int HTDump(const HT* ht) {
	if (HTValidate(ht)) return -1;

	printf(MAGENTA("Hash table:\n"));
	printf(BLUE("Capacity:") " %d\n", ht->capacity);
	printf(BLUE("Hash function:") " <" YELLOW("%p") ">\n", ht->func);

	for (int i = 0; i < ht->capacity; i++) {
		printf(MAGENTA("Bucket #") YELLOW("%d") "\n", i);
		ListTextDump(ht->data[i]);
	}

	return 0;
}

__attribute__((aligned(32))) static char tmp_buf[ELEM_SIZE];

int HTInsert(HT* ht, const char* s) {
#ifdef DEBUG
	if (HTValidate(ht)) return -1;
#endif /* DEBUG */
	
	strncpy(tmp_buf, s, ELEM_SIZE);
	int idx = ht->func(tmp_buf) % ht->capacity;
	if (ListInsert(ht->data[idx], tmp_buf)) {
		return -1;
	}
	
	return 0;
}

// 0 - found, 1 - no
int HTLookUp(const HT* ht, const char* s) {
#ifdef DEBUG
	if (HTValidate(ht)) return -1;
#endif /* DEBUG */

	//radiohead
	//strncpy(tmp_buf, s, ELEM_SIZE);
	/*asm ("vlddqu %1, %%ymm0\n\t"
	"vmovdqa %%ymm0, (%0)\n\t"
	"xor %%rdx, %%rdx\n\t"
	"loop:\n\t"
	"movb (%0, %%rdx, 1), %%bh\n\t"
	"testb %%bh, %%bh\n\t"
	"jz loop2\n\t"
	"inc %%rdx\n\t"
	"cmpq $32, %%rdx\n\t"
	"jnz loop\n\t"
	"jmp end\n\t"

	"loop2:\n\t"
	"inc %%rdx\n\t"
	"movb $0, (%0, %%rdx, 1)\n\t"
	"cmpq $32, %%rdx\n\t"
	"jnz loop2\n\t"

	"end:\n\t"
	:
	: "r" (tmp_buf), "m" (s), "i" (ELEM_SIZE)
	: "%bx", "%rdx");*/

	memset(tmp_buf, 0, ELEM_SIZE);
	asm (
	"xor %%rdx, %%rdx\n\t"
	"loop:\n\t"
	"movb (%0, %%rdx, 1), %%bl\n\t"
	"test %%bl, %%bl\n\t"
	"jz end\n\t"
	"cmp %2, %%rdx\n\t"
	"jz end\n\t"
	"movb %%bl, (%1, %%rdx, 1)\n\t"
	"inc %%rdx\n\t"
	"jmp loop\n\t"

	"end:"
	:
	: "r" (s), "r" (tmp_buf), "i" (ELEM_SIZE - 2)
	: "%rdx", "%bl");
	/*//memcpy(tmp_buf, s, ELEM_SIZE);
	__m256i tmem = _mm256_lddqu_si256((__m256i*) s);
	_mm256_store_si256((__m256i*) tmp_buf, tmem);
	//v trim after zero
	asm (
	"mov $32, %%rdx\n\t"
	"loop1:\n\t"
	"movb (%0, %%rdx, 1), %%bl\n\t"
	"movb $0, (%0, %%rdx, 1)\n\t"
	"dec %%rdx\n\t"
	"cmp $10, %%bl\n\t" //newline
	"jnz loop1\n\t"
	:
	: "r" (tmp_buf), "i" (ELEM_SIZE - 1)
	: "%rdx", "%bl");
//strncpy(tmp_buf, s, ELEM_SIZE);
*/

	int idx = ht->func(tmp_buf) % ht->capacity;
	List* l = ht->data[idx];
	//printf("Searching: |%s| |%s|\n", tmp_buf, s);
	__m256d ms = _mm256_load_pd((const double*) tmp_buf);
	for (int i = 0; i < l->size; i++) {
		//printf("\t Searched: |%s|\n", l->data[i]);
		__m256d msn = _mm256_load_pd((const double*) l->data[i]);
		__m256i mxor = (__m256i) _mm256_xor_pd(ms, msn);
		int res = _mm256_testz_si256(mxor, mxor);
		
		if (res) {
			return 0;
		}
	}
	return 1;
}
