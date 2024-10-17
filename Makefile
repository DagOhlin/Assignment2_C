# Compiler and Linking Variables
CC = gcc
CFLAGS = -Wall -fPIC -lm -fsanitize=thread
LIB_NAME = libmemory_manager.so

# Source and Object Files
SRC = memory_manager.c
OBJ = $(SRC:.c=.o)

# Default target: Build everything
all: mmanager list test_mmanager test_list

# Rule to create the dynamic library
$(LIB_NAME): $(OBJ)
	$(CC) -shared -o $@ $(OBJ)

# Rule to compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build the memory manager
mmanager: $(LIB_NAME)

# Build the linked list
list: linked_list.o

# Test target to build the memory manager test program
test_mmanager: $(LIB_NAME)
	$(CC) -o test_memory_manager test_memory_manager.c -L. -lmemory_manager -lm -g -fsanitize=thread

# Test target to build the linked list test program
test_list: $(LIB_NAME) linked_list.o
	$(CC) -o test_linked_list linked_list.c test_linked_list.c -L. -lmemory_manager -lm -g 
	
# Run all tests
run_tests: run_test_mmanager run_test_list

# Run test cases for the memory manager
run_test_mmanager: test_mmanager
	LD_LIBRARY_PATH=. ./test_memory_manager 
#the 0 is for the arg in the main(arg) to select wich test it is suposed to run 
#LD_LIBRARY_PATH=. valgrind --tool=helgrind ./test_memory_manager for hellgrind

# Run test cases for the linked list
run_test_list: test_list
	LD_LIBRARY_PATH=. ./test_linked_list 0  # Run all tests by default

# Clean target to clean up build files
clean:
	rm -f $(OBJ) $(LIB_NAME) test_memory_manager test_linked_list linked_list.o
