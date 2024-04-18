#include <stdio.h>

#include "strlist.h"


int main() {
	List* l = ListCtor(8);
	
	ListInsert(l, 0, "Hello, world!\n");
	ListInsert(l, 0, "Sus?");
	ListTextDump(l);

	ListDtor(l);
	return 0;
}
