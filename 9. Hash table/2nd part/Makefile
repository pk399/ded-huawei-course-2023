CC = g++

CFLAGS = -O3

INCLUDE = ../../common/include

bench_test: words.txt hf.o
	python run_bench.py

hf.o: hf.asm
	nasm -f elf64 hf.asm -o hf.o

words.txt:
	python process_words.py

strlist.o: strlist.c
	$(CC) $(CFLAGS) -c strlist.c -I$(INCLUDE)

clean:
	rm -r list __pycache__ words.txt
