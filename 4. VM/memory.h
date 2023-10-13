#ifndef _MEMORY_H_
#define _MEMORY_H_

const unsigned DEFAULT_SIZE = 128;
const unsigned DUMP_LIMIT = 20;

typedef struct _memory {
	char* bytes;
	unsigned size;
	unsigned p;
} Memory;

Memory* MemCtor();
int MemDump(Memory*);
int MemDtor(Memory*);

int MemRealloc(Memory*, unsigned new_size);

int MemRead(Memory*, char* byte);
int MemWrite(Memory*, char byte);
int MemSeek(Memory*, unsigned position);

#endif /* _MEMORY_H_ */
