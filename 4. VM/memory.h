#ifndef _MEMORY_H_
#define _MEMORY_H_

typedef struct _memory Memory;

Memory* MemCtor(unsigned elem_size);
int MemDump(Memory*);
int MemDtor(Memory*);

int MemResize(Memory*, unsigned new_size);

// Manipulating by pointer
int MemRead(Memory*, void* elem);
int MemReadPt(Memory*, void* elem, unsigned pointer);
int MemWrite(Memory*, const void* elem);
int MemWritePt(Memory*, const void* elem, unsigned pointer);
int MemEOF(Memory*);
int MemEOFPt(Memory*, unsigned pointer);
int MemShift(Memory*, int position_delta);
int MemSeek(Memory*, unsigned new_position);

// Getters
unsigned MemGetSz(Memory*);
unsigned MemGetPt(Memory*);

#endif /* _MEMORY_H_ */
