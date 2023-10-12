#ifndef _TINY_ONEGIN_H_
#define _TINY_ONEGIN_H_

#include <stdio.h>

struct text6 {
	char* buf;
	char* lines[65536];
};

typedef struct text6 Text;

Text* TextCtor(FILE*);
int TextDtor(Text*);


#endif /* _TINY_ONEGIN_H_ */
