CC = gcc
CFLAGS = -Wall -Wextra -g -I../include -std=c11
VAL = valgrind
VFLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose
LIB_DIR = ..
SRC_DIR = ../src

TEST_SRC = test.c
TEST_EXEC = ./test_pvars
TEST_RUN = $(VAL) $(VFLAGS) $(TEST_EXEC)

LIB_NAME = $(LIB_DIR)/libpvars.a
LIB_SRC_FILES = pdict.c plist.c perrno.c pvars.c
LIB_OBJ_FILES = $(LIB_SRC_FILES:.c=.o)
LIB_OBJS = $(addprefix $(SRC_DIR)/,$(LIB_OBJ_FILES))

all: test
.PHONY: all test clean

test: $(TEST_EXEC)
	@echo "--- Running Test Suite: $(TEST_EXEC) ---"
	$(TEST_RUN)

$(TEST_EXEC): $(TEST_SRC) $(LIB_NAME)
	@echo "Compiling and linking test executable: $@"
	$(CC) $(CFLAGS) $< -o $@ -L$(LIB_DIR) -lpvars -lm

$(LIB_NAME): $(LIB_OBJS)
	@echo "Archiving static library: $@"
	ar rcs $@ $^

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning up local build artifacts..."
	$(RM) $(TEST_EXEC)
	$(RM) $(LIB_OBJS)
	$(RM) $(LIB_NAME)
