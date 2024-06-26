#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <nmmintrin.h>

#include "hash.h"

#ifndef HT_SIZE
#define HT_SIZE 1733
#endif /* HT_SIZE */

#ifdef F1
uint64_t hf(const char* s) {
	return 1;
}
#endif /* F1 */

#ifdef F2
uint64_t hf(const char* s) {
	return s[0];
}
#endif /* F2 */

#ifdef F3
uint64_t hf(const char* s) {
	return strlen(s);
}
#endif /* F3 */

#ifdef F4
uint64_t hf(const char* s) {
	uint64_t sum = 0;

	for (unsigned char* c = (unsigned char*) s; *c != 0; c++) {
		sum += *c;
	}

	return sum;
}
#endif /* F4 */

#ifdef F5
uint64_t hf(const char* s) {
	uint64_t h = 0;

	for (unsigned char* c = (unsigned char*) s; *c != 0; c++) {
		h = (h >> 1) | (h << 63);
		h = h ^ *c;
	}

	return h;
}
#endif /* F5 */

#ifdef F6
uint64_t hf(const char* s) {
	uint64_t h = 0;

	for (unsigned char* c = (unsigned char*) s; *c != 0; c++) {
		h = (h << 1) | (h >> 63);
		h = h ^ *c;
	}

	return h;
}
#endif /* F6 */

#ifdef F7
uint64_t hf(const char* s) {
	uint64_t h = 0;
	char last = 'x';

	for (char* c = (char*) s; *c; c++) {
		h = h << 3;
		h = h + *c;
		h = h ^ (h >> (32 + (*c % 4)));
		h = h ^ ((*c * last) << (last % 42));
		last = *c;
	}

	return h;
}
#endif /* F7 */

#ifdef F8
uint64_t hf(const char* s) {
	uint64_t h = 0;
	
	size_t len = strlen(s);

	for (uint64_t* c = (uint64_t*) s; (char*) c < s + len - 3; c++) {
		h = _mm_crc32_u64(h, *c);
	}

	for (char* c = (char*) s; c < s + len; c++) {
		h = _mm_crc32_u64(h, *c);
	}

	return h;
}
#endif /* F8*/


int main() {
	HT* ht = HTCtor(HT_SIZE, &hf);
	fprintf(stderr, "Created: %p\n", ht);

	HTInsert(ht, "aboa");
	HTInsert(ht, "ys");

	long unsigned int bufsz = 1024;
	char* buf = (char*) malloc(bufsz);

	FILE* f = fopen("words.txt", "rt");
	while (getline(&buf, &bufsz, f) != -1) {
		HTInsert(ht, buf);
	}
	free(buf);
	fclose(f);

	//HTDump(ht);
	
	// print loads
	for (int i = 0; i < ht->capacity; i++) {
		printf("%d ", ht->data[i]->size);
	}

	putchar('\n');

	HTDtor(ht);
	return 0;
}
