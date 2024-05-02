EXEC = printf

main:
	nasm -f elf64 printf.asm # <------ Change if WINDOWS
	gcc -no-pie printf.o main.c -o $(EXEC)

clean:
	rm $(EXEC) printf.o
