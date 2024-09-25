#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "memory_manager.h"
#include"common_defs.h"
#include <string.h>

//make sure the stddef works on linux there are dirrerent printing of size_t for different OS AAAAA
#include <stddef.h>
/*
typedef struct 
{
    void* startAdress;
    size_t size;
    bool isUsed;
} memoryBlock;
*/
// should these be static instead and mabey inside a funkj?
//does not handle 0 size alocation like the test wants 
void* memPoolStart = NULL;
size_t memPoolSize = 0;

memoryBlock* memoryBlocks = NULL;
int memoryBlocksSize = 0;

void increaseMemoryBlockArraySize(memoryBlock** array, size_t BlocksToAdd){


    memoryBlock* temp;

    size_t newSize = memoryBlocksSize * sizeof(memoryBlock) + BlocksToAdd * sizeof(memoryBlock);

    temp = realloc(*array, newSize);

    if(temp == NULL){
        printf("resize failed\n");
        return;
    }

    memoryBlocksSize = memoryBlocksSize + BlocksToAdd;

    *array = temp;
}

size_t sizeLeftAtEndOfPool() {
    if (memoryBlocksSize == 0) {
        
        return memPoolSize;
    } else {
       //om du glömt bort castar du till char för att en char är en byte och det är nödvändigt för att räkna med bytes
       //vet ej dock om det behövs just här 
        memoryBlock lastBlock = memoryBlocks[memoryBlocksSize - 1];
        size_t usedSize = (char*)lastBlock.startAdress + lastBlock.size - (char*)memPoolStart;
        return memPoolSize - usedSize;
    }
}

void mem_init(size_t size){
    
    printf("mem_init\n");
    //add the fail 
    memPoolSize = size;
    void* temp = (void*)malloc(size);

    if(temp == NULL){
        printf("mempool alocation failed\n");
        return;
    }
    memPoolStart = temp;

}





void mem_free(void* block){

    printf("mem_free\n");

    if (block == NULL) {
        printf("cant free is nullpointer\n");
        return;
    }

    
    int index = -1;

    for (int i = 0; i < memoryBlocksSize; i++)
    {
        if(memoryBlocks[i].startAdress == block){
            index = i;
            break;
        }
    }
    
    if(index == -1){

        printf("could not find block with adress to free\n");
        return;


    }

    if(memoryBlocks[index].isUsed == false){
        printf("block is already freed\n");
    }

    printf("Found block to free at index: %d\n", index);

    memoryBlocks[index].isUsed = false;

    if(index > 0 && memoryBlocks[index - 1].isUsed == false){
        printf("found previous block to combine with\n");

        memoryBlocks[index - 1].size += memoryBlocks[index].size;

        // Shift the remaining blocks left
        for (int i = index; i < memoryBlocksSize - 1; i++)
        {
            memoryBlocks[i] = memoryBlocks[i + 1];
        }

        memoryBlocksSize--;

        index--;

    
    }

    
    if(index < memoryBlocksSize - 1 && memoryBlocks[index + 1].isUsed == false){
        printf("found next block to combine with\n");

        memoryBlocks[index].size += memoryBlocks[index + 1].size;

        
        for (int i = index + 1; i < memoryBlocksSize - 1; i++)
        {
            memoryBlocks[i] = memoryBlocks[i + 1];
        }

        memoryBlocksSize--;

    
    }


}

void* mem_alloc(size_t size){
    printf("mem_alloc\n");

    if(size == 0){
        printf("size was 0 returned null\n");
        //this will not return null for now fix when test is updated 
        return memPoolStart;
        
    }

    if(memoryBlocks == NULL){
        
        printf("first allocation started\n");

        if(size <= memPoolSize){
            printf("allocation fits in mempool\n");
            memoryBlocks = (memoryBlock*)malloc(sizeof(memoryBlock));
            memoryBlocksSize += 1;
            memoryBlocks[memoryBlocksSize - 1].size = size;
            memoryBlocks[memoryBlocksSize - 1].isUsed = true;
            memoryBlocks[memoryBlocksSize - 1].startAdress = memPoolStart;
            return memoryBlocks[memoryBlocksSize - 1].startAdress;
        }
        else{
            printf("allocation does not fit in mempool\n");
            return NULL;
        }
    }
    else{
        printf("adding to already used pool\n");

        printf("trying to find existing block that fits\n");

        for (int i = 0; i < memoryBlocksSize; i++){

            if(memoryBlocks[i].isUsed == false && size <= memoryBlocks[i].size){

                printf("found block that fits, it is number: %d\n", i);
                //can t change size of block as that leavs a gap in the blocks for now 
                //memoryBlocks[i].size = size;
                memoryBlocks[i].isUsed = true;

                if(memoryBlocks[i].size != size){
                    printf("memoryblock is larger than needed will split in two\n");

                    increaseMemoryBlockArraySize(&memoryBlocks, 1);

                    if(i < memoryBlocksSize - 1){

                        for (int j = memoryBlocksSize - 1; j > i + 1; j--){
                        
                            memoryBlocks[j] = memoryBlocks[j - 1];
                        }

                    }
                    //same thing here  do not know if the char cast is really nessesary think i do calcs with these elsewhere where i do not use it
                    memoryBlocks[i + 1].startAdress = (char*)memoryBlocks[i].startAdress + size;
                    memoryBlocks[i + 1].size = memoryBlocks[i].size - size;
                    memoryBlocks[i + 1].isUsed = false;
                    memoryBlocks[i].size = size;
                }

                
                
                return memoryBlocks[i].startAdress;

            }

        }
        printf("did not find exisiting pool that could be used\n");
        printf("adding at end of already in use pool\n");

        if(size <= sizeLeftAtEndOfPool()){
            printf("allocation fits in end of mempool\n");

            increaseMemoryBlockArraySize(&memoryBlocks, 1);
            
            memoryBlocks[memoryBlocksSize - 1].size = size;
            memoryBlocks[memoryBlocksSize - 1].isUsed = true;
            memoryBlocks[memoryBlocksSize - 1].startAdress = (char*)memoryBlocks[memoryBlocksSize - 2].startAdress + memoryBlocks[memoryBlocksSize - 2].size;
            return memoryBlocks[memoryBlocksSize - 1].startAdress;
        }
        else{
            printf("allocation does not fit in end of mempool\n");
        }

    }

    

    
    printf("returns nullpointer\n");
    return NULL;
}

void mem_deinit(){

    
    printf("mem_deinit\n");
    free(memPoolStart);
    memPoolStart = NULL;
    free(memoryBlocks);
    memoryBlocks = NULL;

    memoryBlocksSize = 0;
    memPoolSize = 0;

}


void* mem_resize(void* block, size_t size){
    //this can so far only increase the size of the block 
    printf("mem_resize\n");
    int blockIndex;

    for (int i = 0; i < memoryBlocksSize; i++)
    {
        
        if (memoryBlocks[i].startAdress == block)
        {
            printf("found block to resize it is number: %d\n", i);
            blockIndex = i; 

        }
    }    
    // what would i return if i failed to find a block with that adress 
    if(memoryBlocks[blockIndex].size == size){
        printf("block was already that size\n");
        return block; 
    }

    
    //here should be a check for if the new size is smaller but no way to fix the gap in blox that would create right now 


    //ups size of block
    if(blockIndex == memoryBlocksSize - 1 && size <= sizeLeftAtEndOfPool()){
        printf("block is last one and end of pool is enough expanding size\n");
        memoryBlocks[blockIndex].size = size;
        return block;
    }
    printf("looking for new spot for memory\n");
    void* newBockAdress = mem_alloc(size);
    if(newBockAdress == NULL){
        printf("there was no other spot that fit \n");
        //returning block here for now
        return block;
    }

    printf("found new block at new adress\n");
    memcpy(newBockAdress, block, memoryBlocks[blockIndex].size);
    mem_free(block);
    printf("copied over data and freed old block will now return new adress\n");
    return newBockAdress;


    
}

/*
int main(){

    printf("Hello World!\n");

    mem_init(100);

    void* x = mem_alloc(40);
    void* y = mem_alloc(30);
    void* z = mem_alloc(30);

    mem_free(x);
    mem_free(y);
    //mem_free(z);

    

    void* a = mem_alloc(50);
    
    printf("The size is: %Iu\n", memoryBlocks[1].size);
    printf("The size is: %Iu\n", memoryBlocks[2].size);
    

    getchar();
    
}
*/