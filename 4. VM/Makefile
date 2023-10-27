CC = g++
CFLAGS = -O3

INCLUDE = ../common/include
LIB = ../common/lib

all: spu asm

clean:
	rm *.o
	rm spu asm
	
spu: reg.o bcfile.o opcode.o
	$(CC) $(CFLAGS) spu.c $^ -L$(LIB) -lstack -I$(INCLUDE) -o $@

asm: bcfile.o opcode.o
	$(CC) $(CFLAGS) asm.c $^ -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) $< -o $@
