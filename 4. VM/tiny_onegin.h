#ifndef _TINY_ONEGIN_H_
#define _TINY_ONEGIN_H_

#include <stdio.h>

typedef struct text6 Text;

Text* TextCtor(FILE*);
int TextDump(Text*);
int TextDtor(Text*);

char* TextGetLine(Text*, unsigned index);

// Getters
char* TextChars(Text*);
char** TextLines(Text*);
unsigned TextLinesLen(Text*);

#endif /* _TINY_ONEGIN_H_ */
