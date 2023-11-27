EXEC = tree

CFLAGS = -O3

SRCDIR = src/
BUILDDIR = build/

main: latex.include $(addprefix $(BUILDDIR), lang.o main.o tree.o eval.o derivative.o latex.o)
	g++ $(CFLAGS) $(filter %.o,$^) -o $(BUILDDIR)$(EXEC)

latex.include:
	cd $(SRCDIR); python3 latex.py

$(BUILDDIR)%.o: $(SRCDIR)%.c
	g++ $(CFLAGS) -c $^ -o $@
