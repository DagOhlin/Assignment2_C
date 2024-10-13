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

void print_memory_blocks();

void mem_init(size_t size);

void* mem_alloc(size_t size);

void mem_free(void* block);

void mem_deinit(void);

int increaseMemoryBlockArraySize(memoryBlock** array, size_t BlocksToAdd);

size_t sizeLeftAtEndOfPool(void);

void* mem_resize(void* block, size_t size);

#endif 
