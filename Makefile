#------------------------------------------------------------------------------
# PROJECT VARIABLES
#------------------------------------------------------------------------------

# Compiler and Flags
CC      = gcc
# -Wall -Wextra: Enable extensive warnings (highly recommended)
# -g: Include debugging information
# -Iinclude: Tell the compiler to look for header files in the 'include' directory
CFLAGS  = -Wall -Wextra -g -Iinclude

# Archiver and Flags for creating the static library
AR      = ar
ARFLAGS = rcs  # r: insert/replace; c: create archive; s: create index

# Directories
SRC_DIR = src
INC_DIR = include
TEST_DIR = test

# Library definitions
LIB_NAME = libpvars.a
LIB_OBJS = $(SRC_DIR)/perrno.o $(SRC_DIR)/pvars.o


#------------------------------------------------------------------------------
# TARGETS
#------------------------------------------------------------------------------

# Default target: builds the library and the test executable
all: $(LIB_NAME) $(TEST_DIR)/test_pvars
	@echo "Build successful! Run with: ./test/test_pvars"

# Target to create the static library (libpvars.a)
$(LIB_NAME): $(LIB_OBJS)
	@echo "Archiving library: $@"
	$(AR) $(ARFLAGS) $@ $^

# Target to build the test executable
#
# CFLAGS is used for compiling test.c (-Iinclude)
# LDLIBS specifies the linker flags:
# -L.: Look for libraries in the current directory (where libpvars.a is)
# -lpvars: Link against libpvars.a
$(TEST_DIR)/test_pvars: $(TEST_DIR)/test.c $(LIB_NAME)
	@echo "Compiling and linking test executable: $@"
	$(CC) $(CFLAGS) $< -o $@ -L. -lpvars

# Rule for compiling source files in the 'src' directory into object files
# $@ is the target (e.g., src/perrno.o)
# $< is the first prerequisite (e.g., src/perrno.c)
# $(@:.o=.c) converts src/perrno.o to src/perrno.c
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@


#------------------------------------------------------------------------------
# UTILITIES
#------------------------------------------------------------------------------

.PHONY: clean

# Cleans up generated files (object files, library, and test executable)
clean:
	@echo "Cleaning up build artifacts..."
	# Remove object files from src directory
	rm -f $(LIB_OBJS)
	# Remove the static library
	rm -f $(LIB_NAME)
	# Remove the test executable
	rm -f $(TEST_DIR)/test_pvars
