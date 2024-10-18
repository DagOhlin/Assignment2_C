#include "memory_manager.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "common_defs.h"
#include <pthread.h> 
#include <unistd.h>// for sleep and usellep, to simoulate time delay sleep(2) for 2 sec delay
#include <assert.h>




/*



#define TOTAL_ITERATIONS 1000  // Total number of iterations
#define NUM_THREADS 10        // Number of threads

void* threadTestManager(void* arg) {
    int iterations = *(int*)arg;  // Number of iterations per thread
    int iterationsCount = 0;

    for (int i = 0; i < iterations; i++) {
        void* x = mem_alloc(1);
        void* y = mem_alloc(1);

        mem_free(x);
        mem_free(y);

        iterationsCount++;
    }

    printf("Thread %lu completed %d iterations.\n", pthread_self(), iterationsCount);
    return NULL;
}

double get_time_diff(struct timespec start, struct timespec end) {
    double start_sec = (double)start.tv_sec + (double)start.tv_nsec / 1e9;
    double end_sec = (double)end.tv_sec + (double)end.tv_nsec / 1e9;
    return end_sec - start_sec;
}

int main() {
    struct timespec start_time, end_time;
    double multi_thread_time;
    pthread_t threads[NUM_THREADS];
    int iterations_per_thread = TOTAL_ITERATIONS / NUM_THREADS;

    // Initialize the memory pool
    mem_init(1000000);

    clock_gettime(CLOCK_MONOTONIC, &start_time);

    // Create multiple threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, threadTestManager, &iterations_per_thread);
    }
    
    // Wait for all threads to complete
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    multi_thread_time = get_time_diff(start_time, end_time);

    printf("Multi-threaded test time: %f seconds\n", multi_thread_time);
    print_memory_blocks();
    mem_deinit();

    return 0;
}
*/








#define TOTAL_ITERATIONS 100000

void* threadTestManager(void* arg) {
    int iterations = *(int*)arg;  
    
    //pthread_mutex_lock(&Lock);
    int iterationsCount = 0;
    for (int i = 0; i < iterations; i++) {
        
        void* x = mem_alloc(1);
        if (x == NULL){
            assert(0 && "returned addres is null");
        }
        
        
        void* y = mem_alloc(1);
        if (y == NULL){
            assert(0 && "returned addres is null");
        }
        
        if (x == y){
            
            printf_red("startAdressX=%p\n", x);
            printf_red("startAdressY=%p\n", y);
            print_memory_blocks();
            assert(0 && "x = y");
        }
        
        
        
        
        
        
        mem_free(x);
        mem_free(y);

        
        
        iterationsCount++;
        
        
    }
    //pthread_mutex_unlock(&Lock);
    //printf("iterations: %d\n", iterationsCount);
    //printf_red("finnished\n");
    
    return NULL;
}

double get_time_diff(struct timespec start, struct timespec end) {
    double start_sec = (double)start.tv_sec + (double)start.tv_nsec / 1e9;
    double end_sec = (double)end.tv_sec + (double)end.tv_nsec / 1e9;
    return end_sec - start_sec;
}

int main() {
    struct timespec start_time, end_time;
    double single_thread_time, multi_thread_time;
    pthread_t thread1, thread2;
    int iterations_per_thread;
    int total_iterations = TOTAL_ITERATIONS;

    /*
    // Single-threaded test
    mem_init(1000000); // Initialize with a larger pool size
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    // Run the test function in the main thread with TOTAL_ITERATIONS
    threadTestManager(&total_iterations);

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    single_thread_time = get_time_diff(start_time, end_time);
    print_memory_blocks();
    mem_deinit();


    */
    single_thread_time = 0,0;
    printf_green("---------------\n");

    // Multi-threaded test
    mem_init(1000000); // Re-initialize the memory pool
    iterations_per_thread = TOTAL_ITERATIONS / 2;  // Divide the total iterations between 2 threads

    clock_gettime(CLOCK_MONOTONIC, &start_time);

    pthread_create(&thread1, NULL, threadTestManager, &iterations_per_thread);
    pthread_create(&thread2, NULL, threadTestManager, &iterations_per_thread);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    multi_thread_time = get_time_diff(start_time, end_time);
    printf("Multi-threaded test time: %f seconds\n", multi_thread_time);
    printf("Single-threaded test time: %f seconds\n", single_thread_time);
    print_memory_blocks();
    mem_deinit();
    
    return 0;
}



/*
void* threadTestManager(void* address){
    
    printf_yellow("thread stared\n");

    

    void* y = mem_alloc(1);
   
    
    
    return NULL;
}



int main()
{
    
    pthread_t thread1;
    pthread_t thread2;
    pthread_t thread3;
    pthread_t thread4;
    
    mem_init(10000);


    pthread_create(&thread1, NULL, threadTestManager, NULL);
    pthread_create(&thread2, NULL, threadTestManager, NULL);
    pthread_create(&thread3, NULL, threadTestManager, NULL);
    pthread_create(&thread4, NULL, threadTestManager, NULL);
    printf("all threads created\n");

    
    pthread_join(thread1, NULL);
    printf_green("thread1 done\n");
    
    pthread_join(thread2, NULL);
    printf_green("thread2 done\n");

    pthread_join(thread3, NULL);
    printf_green("thread3 done\n");

    pthread_join(thread4, NULL);
    printf_green("thread4 done\n");
    print_memory_blocks();


    mem_deinit();
    
    return 0;
}


*/







