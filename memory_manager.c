#include <stdio.h> //input and output, printf 
#include <stdlib.h>// the libarary for memory mangment(maloc free realloc)
#include <stdbool.h>// for bools 
#include "memory_manager.h"
#include"common_defs.h"//for nice colors 
#include <string.h>
#include <stddef.h>


//Dag Ohlin

/*
general info for memory_manager  

the data struckture for keeping track of the memoryblocks is a struckt 

typedef struct {
    void* startAdress;
    size_t size;
    bool isUsed;
} memoryBlock;

-it keeps track of the where every block starts, the size(in bytes) and if it is in use
-the structs are stored in an array "memoryBlocks" 
-after mem_inti the pool starts of with one big block that is empty(isUsed = false)
-the method for allocation of blocks is "first fit" so the alloc uses the first free block it can 
find(searching from left to right) and uses as much of it as it need, spliting of the rest to be used later
-the manager avoids fragmentation of free memoryblocks by always combining them when they are freed (mem_free())
this makes sure that there are never two free blocks next to eachother 

*/

// global vars are static to avoid access from other files 
//vars for the memorypool
static void* memPoolStart = NULL;
static size_t memPoolSize = 0;

//vars for the array that keeps track of the memoryblocks 
static memoryBlock* memoryBlocks = NULL;
static int memoryBlocksSize = 0;

//prints all memory blocks and their size and if they are in use, only used for 
//debuging and need to be declared in .h for use outside of file
void print_memory_blocks() {
    printf("Current memory blocks:\n");
    for (int i = 0; i < memoryBlocksSize; i++) {
        printf("Block %d: startAdress=%p, size=%zu, isUsed=%s\n", i, memoryBlocks[i].startAdress, memoryBlocks[i].size, memoryBlocks[i].isUsed ? "true" : "false");
    }
}
  

//this function is called by other funcs when a new memoryblock is added 
//it uses realloc to add the amunt of block specefied in BlocksToAdd
//returns 0 on succses -1 on failure 
int increaseMemoryBlockArraySize(memoryBlock** array, size_t BlocksToAdd){


    memoryBlock* temp;
    //calcualtes new size of the array
    size_t newSize = memoryBlocksSize * sizeof(memoryBlock) + BlocksToAdd * sizeof(memoryBlock);

    temp = realloc(*array, newSize);

    //if handles when realloc fails temps is used to not lose address of array
    if(temp == NULL){
        printf("resize failed\n");
        return -1;
    }

    memoryBlocksSize = memoryBlocksSize + BlocksToAdd;

    *array = temp;
    return 0;
}


// intializes the memorypool with malloc acording to size, creates the array 
//for memoryblocks and creats one empty block that takes upp the entire array 
void mem_init(size_t size){
    //mempool setup
    printf("mem_init\n");
    // if a memorypool is already in uses mem_deinit is used to make sure we start from 0
    if(memPoolStart != NULL){
        mem_deinit();
    }

    memPoolSize = size;
    void* temp = (void*)malloc(size);
    //check so that the malloc did not fail
    if(temp == NULL){
        printf("mempool alocation failed\n");
        return;
    }
    memPoolStart = temp;

    //array setup, at start entire pool will be full with one empty block

    memoryBlocks = (memoryBlock*)malloc(sizeof(memoryBlock));
    memoryBlocksSize += 1;
    memoryBlocks[memoryBlocksSize - 1].size = size;
    memoryBlocks[memoryBlocksSize - 1].isUsed = false;
    memoryBlocks[memoryBlocksSize - 1].startAdress = memPoolStart;

}

//mem_free frees the block that "block" is the starting address for and sets it to isUsed = false so it can be used agian
//to avoid fragmentation of free blocks it also looks for adjacent free blocks and combines them
//because this is done every time a block is freed there can never be free blocks next to eachother
//making shure that the mem_alloc and mem_resize can easilly look through the pool for availabe blocks   
//because it is a void func it can t tell the user if the operation has failed 
void mem_free(void* block){

    printf("mem_free\n");
    //check so the address is valid 
    if (block == NULL) {
        printf("cant free is nullpointer\n");
        return;
    }

    
    int index = -1;
    //forloop find the index of "block"
    for (int i = 0; i < memoryBlocksSize; i++)
    {
        if(memoryBlocks[i].startAdress == block){
            index = i;
            break;
        }
    }
    // if index is still -1 after search it will fail because the block with coresponding startadress was not fond 
    if(index == -1){

        printf("could not find block with adress to free\n");
        return;


    }
    // if the block is alreefy free it will fail, althoug the program would still work without this check  
    if(memoryBlocks[index].isUsed == false){
        printf("block is already freed\n");
        return;
    }

    printf("Found block to free at index: %d\n", index);
    
    memoryBlocks[index].isUsed = false;

    //this if will check if the previus block is empty to combine them 
    if(index > 0 && memoryBlocks[index - 1].isUsed == false){
        printf("found previous block to combine with\n");
        //combiens these sizes of the two blocks 
        memoryBlocks[index - 1].size += memoryBlocks[index].size;

        // Shift the remaining blocks left to write over the now nonexisting block
        for (int i = index; i < memoryBlocksSize - 1; i++)
        {
            memoryBlocks[i] = memoryBlocks[i + 1];
        }
        //index and memoryBlocksSize is subtracted by one to represent 
        //that there is now one fewer block in the array
        memoryBlocksSize--;
        
        index--;

    
    }

    //this if will check if the next block is empty to combine them 
    if(index < memoryBlocksSize - 1 && memoryBlocks[index + 1].isUsed == false){
        printf("found next block to combine with\n");
        //combiens these sizes of the two blocks
        memoryBlocks[index].size += memoryBlocks[index + 1].size;

        // Shift the remaining blocks left to write over the now nonexisting block
        for (int i = index + 1; i < memoryBlocksSize - 1; i++)
        {
            memoryBlocks[i] = memoryBlocks[i + 1];
        }

        //memoryBlocksSize is subtracted by one to represent 
        //that there is now one fewer block in the array
        memoryBlocksSize--;

    
    }


}


//looks for unused blocks that fit "size" then sets them as in use
//the alloc uses first fit wich means that it will use the first free block in the pool from the left 
//that can fit the "size"
//if the free block the alloc has found is larger than needet the extra size att the end will become its own free block
//basicly spliting the block in two to make sure no space is wasted
//returns NULL if allocation fails 
void* mem_alloc(size_t size){
    printf("mem_alloc\n");
    //checks so the user is not trying to create a block with 0 bytes
    //it would be useless so returns NULL 
    if(size == 0){
        printf("size was 0 returned null\n");
        
        return NULL;
            
    }
    

    printf("trying to find existing block that fits\n");
    // loops through all of the blocks untill it find one that fits with the if statment 
    for (int i = 0; i < memoryBlocksSize; i++){

        if(memoryBlocks[i].isUsed == false && size <= memoryBlocks[i].size){

            printf("found block that fits, it is number: %d\n", i);
            
            //sets the block being in use to true 
            memoryBlocks[i].isUsed = true;

            //checks if the block needs to be split in two 
            if(memoryBlocks[i].size != size){
                printf("memoryblock is larger than needed will split in two\n");
                //the increaseMemoryBlockArraySize func increases memoryBlocksSize aswell so it is not nessary in this func
                int arrayIncraseReturn = increaseMemoryBlockArraySize(&memoryBlocks, 1);
                //checks so the increaseMemoryBlockArraySize worked
                if(arrayIncraseReturn != 0){
                    //resets the block being in use to false 
                    memoryBlocks[i].isUsed = true;
                    printf("increase of memoryBlockArray faild so allocation fails\n");
                    printf("returns NULL\n");
                    return NULL;
                }

                if(i < memoryBlocksSize - 1){

                    for (int j = memoryBlocksSize - 1; j > i + 1; j--){
                    
                        memoryBlocks[j] = memoryBlocks[j - 1];
                    }

                }
                //cast address to char to nog get Weird behavior in some compilers, it does work without beacause it is void though 
                memoryBlocks[i + 1].startAdress = (char*)memoryBlocks[i].startAdress + size;
                memoryBlocks[i + 1].size = memoryBlocks[i].size - size;
                memoryBlocks[i + 1].isUsed = false;
                memoryBlocks[i].size = size;
            }

            
            
            return memoryBlocks[i].startAdress;

        }

    }
    printf("did not find exisiting block that could be used\n");
    

    

    
    //returns NULL if no block that fit was found 
    
    printf("returns nullpointer\n");
    return NULL;
}

//mem_deinit frees the used memory by the momorypool and frees and resets the array "memoryBlocks" that keeps track of the blocks
//this will make it so the user can start over with a new pool 
void mem_deinit(){

    if(memPoolStart == NULL){
        return;
    }    
    printf("mem_deinit\n");
    free(memPoolStart);
    memPoolStart = NULL;
    free(memoryBlocks);
    memoryBlocks = NULL;

    memoryBlocksSize = 0;
    memPoolSize = 0;
    return;
}

// the mem_resize can resize resizes the specefied block with a new size that can both be larger and smaller
// if the new size is larger it will look if it can combine the current block with the next one, the previus one or both
// if not if will try to find any other free blocks in the pool that will work 
//returns NULL if allocation fails 
void* mem_resize(void* block, size_t size){
    
    printf("mem_resize\n");


    
    //check so the address is valid 
    if (block == NULL) {
        printf("cant free is nullpointer\n");
        return NULL;
    }
    //forloop find the index of "block"
    int blockIndex = -1;

    for (int i = 0; i < memoryBlocksSize; i++)
    {
        
        if (memoryBlocks[i].startAdress == block)
        {
            printf("found block to resize it is number: %d\n", i);
            blockIndex = i; 

        }
    }    
    //if index is still -1 after search it will fail because the block with coresponding startadress was not fond 
    if(blockIndex == -1){

        printf("could not find block with adress to resize\n");
        return NULL;
    }
    //checks if the new size is same as old one, if so it just retunrs the address of "block"
    if(memoryBlocks[blockIndex].size == size){
        printf("block was already that size\n");
        return block; 
    }

    //here it checks if the new size is larger than old one, the func handlse risizes to larger size and smaller size 
    //separatly 
    if(memoryBlocks[blockIndex].size < size){
        printf("new size is larger than old one\n");
        //first if checks if we can resize by just using a free block to the riht, this allows us to not have to move data with memcpy
        //because the startaddres will stay the same and we can skip some unnesseary looping by not having to call mem_alloc 
        if(blockIndex < memoryBlocksSize - 1 && memoryBlocks[blockIndex + 1].isUsed == false 
        && memoryBlocks[blockIndex + 1].size + memoryBlocks[blockIndex].size >= size){
            printf("the next block is free and new size can fit in the two combined\n");
            //recalculates the size of the two blocks 
            memoryBlocks[blockIndex + 1].size = memoryBlocks[blockIndex + 1].size + memoryBlocks[blockIndex].size - size;
            memoryBlocks[blockIndex].size = size;
            memoryBlocks[blockIndex + 1].startAdress = memoryBlocks[blockIndex].size + (char*)memoryBlocks[blockIndex].startAdress;//%
            //i cast to char* because the char is 1 byte so the addition will be correct

            //if the size of free block to the right is 0 after recalculation it will be removed form the "memoryBlocks" array(see free())
            if (memoryBlocks[blockIndex + 1].size == 0)
            {
                printf("resize used upp all of both blocks, will remove block with size 0\n");
                for (int i = blockIndex + 1; i < memoryBlocksSize - 1; i++)
                {
                    memoryBlocks[i] = memoryBlocks[i + 1];
                }

                memoryBlocksSize--;
            }
            
            //returns  "block" as startaddress did not change 
            return block;
            
        }

        //checks if previus block or previus and next block are free and together can ancomadate the resize

        size_t availvble_size = memoryBlocks[blockIndex].size;
        //calcualtes the total available size, the if for the block to the right is in the one for the block to the left 
        //because if left block is not free the case for right block exclusivly has already been handled 
        if(blockIndex > 0 && memoryBlocks[blockIndex - 1].isUsed == false){
            printf("block to the left is free\n");
            availvble_size += memoryBlocks[blockIndex - 1].size;

            if(blockIndex < memoryBlocksSize - 1 && memoryBlocks[blockIndex + 1].isUsed == false){
                printf("block to the left and right is free\n");
                availvble_size += memoryBlocks[blockIndex + 1].size;
            }

        }
        
        //saves the size of the old block for use in memcpy
        size_t oldBlockSize = memoryBlocks[blockIndex].size;

        //checks availvble_size >= size to se if resize is posible  
        if(availvble_size >= size){
            
            //Frees the block to make sure it can be used by the mem_alloc 
            printf("resized block can fit using left block or left and right block\n");
            mem_free(block);
            //allocates an new block of the desired size "size"
            //because of the previus if we know that the mem_alloc will work as we have checked that there is a space large enough
            void* newBlock = mem_alloc(size);
            //checks if the mem_alloc used the sam startaddress, then no memcopy is needed and "block" can be returned 
            if(newBlock == block){
                printf("new bock same as old one, new size\n");
                return block;
            }
            //copies the data/bytes form the old one to the new one  
            memcpy(newBlock, block, oldBlockSize);
            //returns the addres of the new block 
            return newBlock;
        }

        
        

        
        printf("will try to find new block that fits\n");
        //checks if there are any other spotts in the pool it would fit that where not adjacant 
        //tries to allocate a new block with the desired size "size"
        void* newBockAdress = mem_alloc(size);
        //checks if the alloc failed 
        if(newBockAdress == NULL){
            printf("there was no other spot that fit, returns null\n");
           
            return NULL;
        } 

        printf("found new block at new adress\n");
        //copies over the data 
        memcpy(newBockAdress, block, oldBlockSize);
        //frees the old block 
        mem_free(block);
        printf("copied over data and freed old block will now return new adress\n");
        return newBockAdress;
        

    }
    //here we check if the the new size is smaller than the old one 
    // we do it in a simmilar way to before where we free the old block
    //then alloc a new because we can be sure that there is alteast one spot it will fit 
    //then we copy over the data/bytes 
    if(memoryBlocks[blockIndex].size > size){
        
        printf("new size is smaller than old one freeing block\n");
        mem_free(block);
        void* newBlock = mem_alloc(size);

        if(newBlock == block){
            printf("new bock same as old one, just smaller size\n");
            return block;
        }

        memcpy(newBlock, block, size);
        return newBlock;
        
    }
    //if all alternatives have failed we retun NULL
    printf("resize failed\n");
    return NULL;

    
}


