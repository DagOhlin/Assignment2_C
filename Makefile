# Compiler and Linking Variables
CC = gcc
CFLAGS = -Wall -fPIC -lm  -g -pthread
LIB_NAME = libmemory_manager.so

# Source and Object Files
SRC = memory_manager.c
OBJ = $(SRC:.c=.o)

# Default target: Build everything
all: mmanager list test_mmanager test_list

# Rule to create the dynamic library
$(LIB_NAME): $(OBJ)
	$(CC) -shared -o $@ $(OBJ) -pthread

# Rule to compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build the memory manager
mmanager: $(LIB_NAME)

# Build the linked list
list: linked_list.o

# Test target to build the memory manager test program
test_mmanager: $(LIB_NAME)
	$(CC) -o test_memory_manager test_memory_manager.c -L. -lmemory_manager -lm -g -pthread 
#$(CC) -o test_memory_manager test_memory_manager.c -L. -lmemory_manager -lm -g -fsanitize=thread

# Test target to build the linked list test program
test_list: $(LIB_NAME) linked_list.o
	$(CC) -o test_linked_list linked_list.c test_linked_list.c -L. -lmemory_manager -lm -g -pthread 
	cp test_linked_list test_linked_listCG
	
# Run all tests
run_tests: run_test_mmanager run_test_list

# Run test cases for the memory manager
run_test_mmanager: test_mmanager
	LD_LIBRARY_PATH=.  ./test_memory_manager 0
#the 0 is for the arg in the main(arg) to select wich test it is suposed to run 
#LD_LIBRARY_PATH=. valgrind --tool=helgrind ./test_memory_manager for hellgrind
#LD_LIBRARY_PATH=. TSAN_OPTIONS="verbosity=2 log_path=tsan_log" ./test_memory_manager 
#TSAN_OPTIONS="force_seq_cst_atomics=1 verbosity=1 history_size=6"

# Run test cases for the linked list
run_test_list: test_list
	LD_LIBRARY_PATH=. ./test_linked_list 0  # Run all tests by default

# Clean target to clean up build files
clean:
	rm -f $(OBJ) $(LIB_NAME) test_memory_manager test_linked_list linked_list.o
