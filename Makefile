# --- Makefile for libpvars C Library ---

# Compiler and Flags
CC = gcc
# -Wall, -Wextra: Enable all common and extra warnings
# -g: Include debugging information (GDB)
# -Iinclude: Add the 'include' directory to the compiler's search path for headers
# -std=c11: Compile using the C11 standard
CFLAGS = -Wall -Wextra -g -Iinclude -std=c11

# Directories and Files
SRC_DIR = src
TEST_DIR = test
LIB_NAME = libpvars.a

# Source and Object files for the Library
SRC_FILES = pdict.c plist.c perrno.c
OBJ_FILES = $(SRC_FILES:.c=.o)
OBJS = $(addprefix $(SRC_DIR)/,$(OBJ_FILES))

# Test Executable and Source
TEST_SRC = $(TEST_DIR)/pdict_test.c
TEST_EXEC = $(TEST_DIR)/test_pdict

# -----------------
# PRIMARY TARGETS
# -----------------

# Default target: Builds the library and the test executable
all: $(LIB_NAME) $(TEST_EXEC)
.PHONY: all

# -----------------
# LIBRARY BUILD
# -----------------

# Rule to archive object files into the static library
$(LIB_NAME): $(OBJS)
	@echo "Archiving library: $@"
	ar rcs $@ $^

# Rule to compile source files into object files
# This is a pattern rule that applies to all files matching src/*.c
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

# -----------------
# TEST BUILD
# -----------------

# Rule to compile and link the test executable
$(TEST_EXEC): $(TEST_SRC) $(LIB_NAME)
	@echo "Compiling and linking test executable: $@"
	$(CC) $(CFLAGS) $< -o $@ -L. -l$(patsubst lib%.a,%,$(LIB_NAME))

# Target to run the tests (assumes successful build)
test: $(TEST_EXEC)
	@echo "Build successful! Running tests..."
	@./$(TEST_EXEC)
.PHONY: test

# -----------------
# CLEANUP
# -----------------

# Remove all generated files
clean:
	@echo "Cleaning project files..."
	rm -f $(OBJS) $(LIB_NAME) $(TEST_EXEC)
.PHONY: clean
