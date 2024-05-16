#ifndef _TEXT_H_
#define _TEXT_H_

#include <stdio.h>

typedef struct _text Text;

int texpect(Text* t, const char* s); // Check if next chars start with 's':
				     // 	no - return 1
				     // 	yes - move pointer, return 0
int tnewlined(Text* t); // Check if pointer stands after newline char
int tparseid(Text* t, char** s); // Dynamically allocate buffer and return id.
			        // Retuns: 1 - parsed, 0 - not
int tparsenum(Text* t, long unsigned* n); // Same as parse id
char ttrychar(Text* t, const char* s); // Return char from the provided ones if encountered, otherwise return '\0'

int tsave(Text* t); // Save ptr (only one slot)
int trollback(Text* t); // Go back to saved pointer

int tprintf(Text* t, const char* fmt, ...);
int tinsert(Text* t, const char* s);
const char* tstr(Text* t);
int texport(FILE* f, Text* t);

Text* tctor(unsigned size);
Text* tfromstr(const char* s);
Text* tblank();
int tdump(Text* t);
int tdtor(Text* t);

#endif /* _TEXT_H_ */
