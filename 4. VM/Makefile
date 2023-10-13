CC = g++
CFLAGS = -O3

all: vm asm

clean:
	rm *.o
	rm vm asm
	
vm: func_stack.o memory.o bytecode.o cmd.o
	$(CC) $(CFLAGS) vm.c $^ -o $@

asm: memory.o tiny_onegin.o cmd.o bytecode.o
	$(CC) $(CFLAGS) assembler.c $^ -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) $< -o $@
