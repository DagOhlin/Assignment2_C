# Compiler and Linking Variables
CC = gcc
CFLAGS = -Wall -fPIC -lm  -g -pthread
LIB_NAME = libmemory_manager.so

# Source and Object Files
SRC = memory_manager.c
OBJ = $(SRC:.c=.o)

# Default target: Build everything
all: mmanager list

# Rule to create the dynamic library
$(LIB_NAME): $(OBJ)
	$(CC) -shared -o $@ $(OBJ) -pthread

# Rule to compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build the memory manager
mmanager: $(LIB_NAME)

# Rule to build the linked list application and link with memory manager
list: linked_list.o #$(LIB_NAME)
#$(CC) -o linked_list linked_list.o -L. -lmemory_manager -pthread -lm -g


# Test target to build the memory manager test program
test_mmanager: $(LIB_NAME)
	$(CC) -o test_memory_manager test_memory_manager.c -L. -lmemory_manager -lm -g -pthread 
#$(CC) -o test_memory_manager test_memory_manager.c -L. -lmemory_manager -lm -g -fsanitize=thread

# Test target to build the linked list test program
test_list: $(LIB_NAME) linked_list.o
	$(CC) -o test_linked_list linked_list.c test_linked_list.c -L. -lmemory_manager -lm -g -pthread 
# Run all tests


# Clean target to clean up build files
clean:
	rm -f $(OBJ) $(LIB_NAME) test_memory_manager test_linked_list linked_list.o linked_list
