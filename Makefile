# --- Makefile for libpvars C Library (Official Version) ---

CC = gcc
CFLAGS = -Wall -Wextra -g -fPIC -Iinclude -std=c11

SRC_DIR = src
LIB_NAME = libpvars.a

SRC_FILES = pdict.c plist.c perrno.c pvars.c
OBJ_FILES = $(SRC_FILES:.c=.o)
OBJS = $(addprefix $(SRC_DIR)/,$(OBJ_FILES))


all: $(LIB_NAME)
.PHONY: all

$(LIB_NAME): $(OBJS)
	@echo "Archiving static library: $@"
	ar rcs $@ $^


$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	@echo "Cleaning up build artifacts..."
	$(RM) $(OBJS) $(LIB_NAME)
.PHONY: clean
