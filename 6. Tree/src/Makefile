EXEC = tree

CFLAGS = -O3

main: lang.o main.o tree.o
	g++ $(CFLAGS) $^ -o $(EXEC)

%.o: %.c
	g++ $(CFLAGS) -c $^
