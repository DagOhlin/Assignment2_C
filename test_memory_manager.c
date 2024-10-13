#include "memory_manager.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "common_defs.h"
#include <pthread.h> 



void* threadTestManager(void* address){
    
    printf("thread stared\n");

}



int main()
{
    
    pthread_t thread1;
    pthread_t thread2;
    

    pthread_create(&thread1, NULL, threadTestManager, NULL);
    pthread_create(&thread2, NULL, threadTestManager, NULL);
    printf("hello\n");
    
    return 0;
}
