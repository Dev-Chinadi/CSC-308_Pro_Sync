#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/time.h>

#define NUM_THREADS 6

// Synchronization tools.
pthread_mutex_t my_mutex;
sem_t counting_semaphore;

// Shared resource simulation.
void use_resource(int thread_id, const char* sync_type) {
    // Thread sleeps for 1 second.
    printf("Thread %d entered critical section using %s\n", thread_id, sync_type);
    sleep(1); 
    printf("Thread %d leaving critical section using %s\n", thread_id, sync_type);
}

// MUTEX APPROACH (1 Thread at a time).
void *mutex_task(void *arg) {
    int thread_id = *(int *)arg;
    
    pthread_mutex_lock(&my_mutex);
    use_resource(thread_id, "MUTEX");
    pthread_mutex_unlock(&my_mutex);
    
    return NULL;
}

// SEMAPHORE APPROACH (N Threads at a time).
void *semaphore_task(void *arg) {
    int thread_id = *(int *)arg;
    
    sem_wait(&counting_semaphore);
    use_resource(thread_id, "SEMAPHORE");
    sem_post(&counting_semaphore);
    
    return NULL;
}

//  Calculate time
double get_time() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (t.tv_sec) + (t.tv_usec / 1000000.0);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    double start_time, end_time;

    printf(" ---- Practical 3: Mutex vs. Counting Semaphore. ---- \n\n");

    // Initialize tools.
    pthread_mutex_init(&my_mutex, NULL);
    sem_init(&counting_semaphore, 0, 3); 

    // TEST 1: MUTEX LOCK.
    printf("Mutex Test (1 Thread at a time)...\n");
    start_time = get_time();
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, mutex_task, &thread_ids[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    end_time = get_time();
    printf("Mutex total time: %.2f seconds.\n", end_time - start_time);

    // TEST 2: COUNTING SEMAPHORE.
    printf("\nCounting Semaphore Test (3 Threads simultaneously)...\n");
    start_time = get_time();
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, semaphore_task, &thread_ids[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    end_time = get_time();
    printf("Semaphore total time: %.2f seconds.\n", end_time - start_time);

    // Clean up
    pthread_mutex_destroy(&my_mutex);
    sem_destroy(&counting_semaphore);

    return 0;
}
