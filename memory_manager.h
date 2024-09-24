#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h>  // For size_t
#include <stdbool.h> // For bool

// Struct definition for a memory block
typedef struct {
    void* startAdress;
    size_t size;
    bool isUsed;
} memoryBlock;

// Function declarations

// Initializes the memory pool with the specified size
void mem_init(size_t size);

// Allocates a block of memory of the specified size from the memory pool
void* mem_alloc(size_t size);

// Frees the specified block of memory
void mem_free(void* block);

// Deinitializes the memory pool and frees all allocated memory
void mem_deinit(void);

// Function to increase the size of the memory block array
void increaseMemoryBlockArraySize(memoryBlock** array, size_t BlocksToAdd);

// Calculates the size left at the end of the pool
size_t sizeLeftAtEndOfPool(void);

//does nothing as of now
void* mem_resize(void* block, size_t size);

#endif // MEMORY_MANAGER_H
