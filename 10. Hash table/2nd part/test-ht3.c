#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#include "gettime.h"

#include "hash3.h"
#include "strlist2.h"


extern "C" uint64_t hf(list_t s);


int main(int argc, char** argv) {
	assert(argc > 1);
	int word_count = atoi(argv[1]);
	assert(argc > word_count + 1);

	fprintf(stderr, "Running opt2\n");

	HT* ht = HTCtor(1733, &hf);
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

	uint64_t start_time = gettime();
	for (int i = 0; i < word_count; i++) {
		int res = HTLookUp(ht, argv[i+2]);
		//printf("%d\n", res);
	}
	uint64_t end_time = gettime();
	printf("%lu\n", end_time - start_time);

	HTDtor(ht);
	return 0;
}
