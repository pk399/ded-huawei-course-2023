#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <immintrin.h>

#include "strlist2.h"
#include "colors.h"

#include "hash3.h"


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

	for (int i = 0; i < ht->capacity; i++) {
		if (ListValidate(ht->data[i]) == -1) return -1;
	}

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
	if (ListInsert(ht->data[idx], 0, tmp_buf)) {
		return -1;
	}
	
	return 0;
}

// 0 - found, 1 - no
int HTLookUp(const HT* ht, const char* s) {
#ifdef DEBUG
	if (HTValidate(ht)) return -1;
#endif /* DEBUG */

	strncpy(tmp_buf, s, ELEM_SIZE);
	int idx = ht->func(tmp_buf) % ht->capacity;
	List* l = ht->data[idx];
	//printf("Searching: %s\n", tmp_buf);
	__m256d ms = _mm256_load_pd((const double*) tmp_buf);
	for (int node = l->next[0]; node != 0; node = l->next[node]) {
		//printf("\t Searched: %s\n", l->data[node]);
		__m256d msn = _mm256_load_pd((const double*) l->data[node]);
		__m256i mxor = _mm256_castpd_si256(_mm256_xor_pd(ms, msn));
		int res = _mm256_testz_si256(mxor, mxor);

		if (res) {
			return 0;
		}
	}
	return 1;
}
