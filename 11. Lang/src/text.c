#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include "colors.h"

#include "text.h"

const char WHITESPACE[] = u8"\n \t\r";

const char IDCHARS[][4] = {u8"а", u8"б", u8"в", u8"г", u8"д", u8"е", u8"ё", u8"ж", u8"з", u8"и", u8"й", u8"к", u8"л", u8"м", u8"н", u8"о", u8"п", u8"р", u8"с", u8"т", u8"у", u8"ф", u8"х", u8"ц", u8"ч", u8"ш", u8"щ" , u8"ъ", u8"ы", u8"ь", u8"э", u8"ю", u8"я", u8"0", u8"1", u8"2", u8"3", u8"4", u8"5", u8"6", u8"7", u8"8", u8"9", u8"0"};
const int IDCHARSSZ = sizeof(IDCHARS) / sizeof(IDCHARS[0]);

const unsigned DEFAULTBLANKSZ = 2048;
const unsigned WRITEBUFSZ = 2048; // Increase if needed

struct _text {
	char* buf;
	bool* newline; // is char after newline?
	bool* whitespace; // is char after whitespace?
	
	char* idbuf; // temp location for id

	int size;
	int p; // pointer
	int save_p;
};


int _tresize(Text* t, unsigned newsize) {
	assert(newsize > t->size);

	t->buf = realloc(t->buf, newsize * sizeof(char));
	t->idbuf = realloc(t->idbuf, newsize * sizeof(char));
	t->newline = realloc(t->newline, newsize * sizeof(bool));
	t->whitespace = realloc(t->whitespace, newsize * sizeof(bool));

	if (!t->buf || !t->newline || !t->idbuf || !t->whitespace) {
		tdtor(t);
		
		return -1;
	}

	#define MEMSET(PTR, LASTSIZE, NEWSIZE, ELEM) memset(PTR + LASTSIZE*sizeof(ELEM), 0, (NEWSIZE - LASTSIZE)*sizeof(ELEM));
	MEMSET(t->buf, t->size, newsize, char);
	MEMSET(t->idbuf, t->size, newsize, char);
	MEMSET(t->newline, t->size, newsize, bool);
	MEMSET(t->whitespace, t->size, newsize, bool);
	#undef MEMSET

	t->size = newsize;

	return 0;
}


Text* tctor(unsigned size) {
	Text* t = calloc(1, sizeof(Text));
	if (!t) return NULL;

	if (_tresize(t, size)) {
		return NULL;
	}

	return t;
}


Text* tfromstr(const char* s) {
	Text* t = tctor(strlen(s));
	if (!t) return NULL;
	
	tinsert(t, s);
	t->p = 0;

	// TODO: lowercase, maybe

	return t;
}


Text* tblank() {
	Text* t = tctor(DEFAULTBLANKSZ);

	return t;
}


int tdump(Text* t) {
	printf(MAGENTA("Text") " (p = %d, size = %d):\n\t", t->p, t->size);

	for (int i = 0; t->buf[i]; i++) {
		if (t->newline[i]) {
			printf(YELLOW("\\n"));
		}
		if (t->whitespace[i]) {
			printf(GREEN("_"));
		}
		if (t->p == i) {
			printf(BLUE("^"));
		}
		putchar(t->buf[i]);
	}

	putchar('\n');
}


int tdtor(Text* t) {
	if (t) {
		if (t->buf) free(t->buf);
		if (t->newline) free(t->newline);
		if (t->idbuf) free(t->idbuf);
		if (t->whitespace) free(t->whitespace);

		memset(t, 0, sizeof(Text));
		free(t);
	}
}


int tsave(Text* t) {
	t->save_p = t->p;
}


int trollback(Text* t) {
	t->p = t->save_p;
}


int tprintf(Text* t, const char* fmt, ...) {
	char tbuf[WRITEBUFSZ];
	va_list va;
	va_start(va, fmt);
	int len = vsnprintf(tbuf, WRITEBUFSZ, fmt, va);
	va_end(va);

	int res = tinsert(t, tbuf);

	return res;
}


int tinsert(Text* t, const char* s) {
	for ( ; *s; s++) {
		char* wp = NULL;
		if (wp = strchr(WHITESPACE, *s)) {
			if (*wp == '\n') {
				t->newline[t->p] = 1;
			}
			t->whitespace[t->p] = 1;
		} else { // not a whitespace
			t->buf[t->p] = *s;
			t->p++;
		}

		if (t->p == t->size) {
			if (_tresize(t, t->size * 2)) {
				return -1;
			}
		}
	}
}


const char* tstr(Text* t) {
	return t->buf;
}


int texport(FILE* f, Text* t) {
	for (int i = 0 ; i != t->p ; i++) {
		if (t->newline[i]) {
			fputc('\n', f);
		} else if (t->whitespace[i]) {
			fputc(' ', f);
		}

		fputc(t->buf[i], f);
	}
}

// strncmp(s1, s2, i)
int texpect(Text* t, const char* s) {
	size_t len = strlen(s);
	if (!strncmp(t->buf + t->p, s, len)) {
		t->p += len;
		return 0;
	}

	return 1;
}


int tnewlined(Text* t) {
	return t->newline[t->p];
}


int tparseid(Text* t, char** s) {
	//if (t->buf[t->p] != '\xd0') { // should be russian letter
	//	*s = NULL; 
	//	return 0;
	//}
	// TODO, maybe first char

	int run = 1;
	char* p = t->idbuf;
	*p = '\0';
	while (run) {
		run = 0;

		for (int i = 0; i < IDCHARSSZ; i++) {
			size_t len = strlen(IDCHARS[i]);
			if (!strncmp(t->buf + t->p, IDCHARS[i], len)) {
				strcpy(p, IDCHARS[i]);
				p += strlen(p);
				t->p += len;

				run = 1;
				break;
			}
		}

		// Next char is after wp?
		if (t->whitespace[t->p]) break;
	}
	
	//printf("BIG PARSE: |%s|\n", t->idbuf);
	if (strlen(t->idbuf) == 0) {
		*s = NULL;
		return 0;
	}

	*s = strdup(t->idbuf);

	return 1; 
}


int tparsenum(Text* t, long unsigned* n) {
	char l = ttrychar(t, u8"123456789"); // letter (c)
	if (!l) { return 0; }

	*n = 0;

	while (l) {
		*n *= 10;
		*n += l - '0';

		if (t->whitespace[t->p]) {
			break;
		}

		l = ttrychar(t, u8"0123456789");
	}

	return 1;
}


char ttrychar(Text* t, const char* s) {
	for ( ; *s; s++) {
		if (*s == t->buf[t->p]) {
			t->p += 1;
			return *s;
		}
	}

	return '\0';
}
