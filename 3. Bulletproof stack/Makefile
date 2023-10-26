TARGET_EXEC = stack

SRC_DIR   = src/
BUILD_DIR = build/

CC = g++
CFLAGS = -O3

SRC_FILES := stack.cpp stack_debug.cpp stack_canary.cpp stack_hash.cpp
OBJ_FILES := $(patsubst %.cpp,%.o,$(SRC_FILES))

lib: build_dir $(OBJ_FILES)
	ar rvs $(BUILD_DIR)lib$(TARGET_EXEC).a	 $(addprefix $(BUILD_DIR),$(filter %.o,$^))
#	$(CC) $(addprefix $(BUILD_DIR),$(OBJ_FILES)) -o $@

build_dir:
	mkdir -p build

install: lib
	cp $(BUILD_DIR)lib$(TARGET_EXEC).a ../common/lib/
	python3 hackinclude.py $(SRC_DIR)config.h $(SRC_DIR)stack_include.h > ../common/include/$(TARGET_EXEC).h
	
uninstall:
	rm ../common/lib/lib$(TARGET_EXEC).a
	rm ../common/include/$(TARGET_EXEC).h

clean:
	rm -r $(BUILD_DIR)
	
$(OBJ_FILES): %.o: $(SRC_DIR)%.cpp build_dir
	$(CC) -c $(CFLAGS) $< -o $(BUILD_DIR)$@
