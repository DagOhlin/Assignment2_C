#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "memory_manager.h"
#include"common_defs.h"
#include <string.h>
// the code can not handle combining a free block and end of pool so i just start with 
// a free block the size of the pool but the code for a free pool still workes  
//half of the code is useless now but meh

//the free has a find block wich checks for invalid addreses maby add that to the others? 
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

void print_memory_blocks() {
    printf("Current memory blocks:\n");
    for (int i = 0; i < memoryBlocksSize; i++) {
        printf("Block %d: startAdress=%p, size=%zu, isUsed=%s\n", i, memoryBlocks[i].startAdress, memoryBlocks[i].size, memoryBlocks[i].isUsed ? "true" : "false");
    }
}



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

    //new code for startfull test below 

    memoryBlocks = (memoryBlock*)malloc(sizeof(memoryBlock));
    memoryBlocksSize += 1;
    memoryBlocks[memoryBlocksSize - 1].size = size;
    memoryBlocks[memoryBlocksSize - 1].isUsed = false;
    memoryBlocks[memoryBlocksSize - 1].startAdress = memPoolStart;

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
        
        return NULL;
        
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
    
    printf("mem_resize\n");

    int blockIndex = -1;

    for (int i = 0; i < memoryBlocksSize; i++)
    {
        
        if (memoryBlocks[i].startAdress == block)
        {
            printf("found block to resize it is number: %d\n", i);
            blockIndex = i; 

        }
    }    

    if(blockIndex == -1){

        printf("could not find block with adress to resize\n");
        return NULL;


    }

    if(memoryBlocks[blockIndex].size == size){
        printf("block was already that size\n");
        return block; 
    }


    if(memoryBlocks[blockIndex].size < size){
        printf("new size is larger than old one\n");
         
        if(blockIndex < memoryBlocksSize - 1 && memoryBlocks[blockIndex + 1].isUsed == false 
        && memoryBlocks[blockIndex + 1].size + memoryBlocks[blockIndex].size >= size){
            printf("the next block is free and new size can fit in the two combined\n");
            //why this part is done diffently from looking at the previus block is to avoid unnecesary looping 
            memoryBlocks[blockIndex + 1].size = memoryBlocks[blockIndex + 1].size + memoryBlocks[blockIndex].size - size;
            memoryBlocks[blockIndex].size = size;
            memoryBlocks[blockIndex + 1].startAdress = memoryBlocks[blockIndex].size + memoryBlocks[blockIndex].startAdress;
            //should i cast to char here?

            if (memoryBlocks[blockIndex + 1].size == 0)
            {
                printf("resize used upp all of both blocks, will remove block with size 0\n");
                for (int i = blockIndex + 1; i < memoryBlocksSize - 1; i++)
                {
                    memoryBlocks[i] = memoryBlocks[i + 1];
                }

                memoryBlocksSize--;
            }
            
            
            return block;
            //this risks creating 0 size blocks which should be fine and will disaprear with time but will make iteration longer for no reason
        }

        //checks if previus block or previus and next block are free and together can ancomadate the resize

        size_t availvble_size = memoryBlocks[blockIndex].size;

        if(blockIndex > 0 && memoryBlocks[blockIndex - 1].isUsed == false){
            printf("block to the left is free\n");
            availvble_size += memoryBlocks[blockIndex - 1].size;

            if(blockIndex < memoryBlocksSize - 1 && memoryBlocks[blockIndex + 1].isUsed == false){
                printf("block to the left and right is free\n");
                availvble_size += memoryBlocks[blockIndex + 1].size;
            }

        }

        //printf("availvble_size: %d\n", availvble_size);

        if(availvble_size >= size){


            printf("resized block can fit using left block or left and right block\n");
            mem_free(block);
            void* newBlock = mem_alloc(size);

            if(newBlock == block){
                printf("new bock same as old one, new size\n");
                return block;
            }

            memcpy(newBlock, block, size);
            return newBlock;
        }

        //checks if there are any other spotts in the pool it would fit 
        if(blockIndex == memoryBlocksSize - 1 && size - memoryBlocks[blockIndex].size <= sizeLeftAtEndOfPool()){
            printf("block is last one and end of pool is enough expanding size\n");
            memoryBlocks[blockIndex].size = size;
            return block;
        }   

        
        printf("will try to find new block that fits\n");

        void* newBockAdress = mem_alloc(size);
        if(newBockAdress == NULL){
            printf("there was no other spot that fit, returns null\n");
           
            return NULL;
        } 

        printf("found new block at new adress\n");
        memcpy(newBockAdress, block, size);
        mem_free(block);
        printf("copied over data and freed old block will now return new adress\n");
        return newBockAdress;
        

    }

    if(memoryBlocks[blockIndex].size > size){
        //unnessesary if, I know
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


    return NULL;

    
}


int main(){

    printf("Hello World!\n");

    mem_init(100);

    print_memory_blocks();

    
    void* x = mem_alloc(40);
    void* y = mem_alloc(40);
    void* z = mem_alloc(20);
    print_memory_blocks();

    mem_free(x);
    

    print_memory_blocks();

    y = mem_resize(y, 70);

    mem_free(z);

    print_memory_blocks();

    //dw
   
    
    //--

    mem_deinit();

    getchar();
    
    return 0;
    
}

