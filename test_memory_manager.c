#include "memory_manager.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "common_defs.h"
#include <pthread.h> 
#include <unistd.h>// for sleep and usellep, to simoulate time delay sleep(2) for 2 sec delay



void* threadTestManager(void* address){
    
    printf_yellow("thread stared\n");

    void* x = mem_alloc(30);
    x = mem_resize(x, 40);
    void* y = mem_alloc(9);
    y = mem_resize(y, 11);
    y = mem_resize(y, 1);
    
    
    
    return NULL;
}



int main()
{
    
    pthread_t thread1;
    pthread_t thread2;
    
    mem_init(100);


    pthread_create(&thread1, NULL, threadTestManager, NULL);
    pthread_create(&thread2, NULL, threadTestManager, NULL);
    printf("all threads created\n");

    
    pthread_join(thread1, NULL);
    printf_green("thread1 done\n");
    
    pthread_join(thread2, NULL);
    printf_green("thread2 done\n");
    print_memory_blocks();


    mem_deinit();
    
    return 0;
}
