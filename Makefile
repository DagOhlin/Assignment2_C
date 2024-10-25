CC = gcc
CFLAGS = -Wall -fPIC -lm  -g -pthread
LIB_NAME = libmemory_manager.so

SRC = memory_manager.c
OBJ = $(SRC:.c=.o)

all: mmanager test_mmanager test_list list

$(LIB_NAME): $(OBJ)
	$(CC) -shared -o $@ $(OBJ) -pthread

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

mmanager: $(LIB_NAME)

#I only make linked list.o file because linked list does not have a main 
list: linked_list.o #$(LIB_NAME)
#$(CC) -o linked_list linked_list.o -L. -lmemory_manager -pthread -lm -g

test_mmanager: $(LIB_NAME)
	$(CC) -o test_memory_manager test_memory_manager.c -L. -lmemory_manager $(CFLAGS)
#$(CC) -o test_memory_manager test_memory_manager.c -L. -lmemory_manager -lm -g -fsanitize=thread


test_list: $(LIB_NAME) linked_list.o
	$(CC) -o test_linked_list linked_list.c test_linked_list.c -L. -lmemory_manager $(CFLAGS) 

clean:
	rm -f $(OBJ) $(LIB_NAME) test_memory_manager test_linked_list linked_list.o linked_list
