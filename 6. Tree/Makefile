EXEC = tree

CFLAGS = -O3

SRCDIR = src/
BUILDDIR = build/

main: $(addprefix $(BUILDDIR), lang.o main.o tree.o)
	g++ $(CFLAGS) $^ -o $(BUILDDIR)$(EXEC)

$(BUILDDIR)%.o: $(SRCDIR)%.c
	g++ $(CFLAGS) -c $^ -o $@
