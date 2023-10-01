TARGET_EXEC = stack

CC = g++
CFLAGS = -O3

SRC_FILES = main.cpp stack.cpp stack_debug.cpp stack_canary.cpp stack_hash.cpp
OBJ_FILES = $(patsubst %.cpp,%.o,$(SRC_FILES))

$(TARGET_EXEC): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $@
	
clean:
	rm *.o
	rm $(TARGET_EXEC)
	
%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@
