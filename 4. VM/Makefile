CC = g++
CFLAGS = -O3

INCLUDE = ../common/include
LIB = ../common/lib

all: spu asm

clean:
	rm *.o
	rm spu asm
	
spu: spu.c reg.o bcfile.o opcode.o
	$(CC) $(CFLAGS) $^ -L$(LIB) -lstack -I$(INCLUDE) -o $@

asm: asm.c bcfile.o opcode.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) $< -o $@
