// producer_consumer_practical2.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define ITEMS_TO_PRODUCE 15

int buffer[BUFFER_SIZE];
int in = 0; // Index for the producer.
int out = 0; // Index for the consumer.

sem_t empty_slots;
sem_t full_slots;
pthread_mutex_t buffer_mutex;

void* producer(void* arg) {
    for (int i = 1; i <= ITEMS_TO_PRODUCE; i++) {
        sem_wait(&empty_slots);
        pthread_mutex_lock(&buffer_mutex);

        buffer[in] = i;
        printf("Produced item: %d at index %d\n", i, in);
        in = (in + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&buffer_mutex);
        sem_post(&full_slots);

        sleep(1);
    }
    return NULL;
}

void* consumer(void* arg) {
    for (int i = 1; i <= ITEMS_TO_PRODUCE; i++) {
        sem_wait(&full_slots);
        pthread_mutex_lock(&buffer_mutex);

        int item = buffer[out];
        printf("Consumed item: %d from index %d\n", item, out);
        out = (out + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&buffer_mutex);
        sem_post(&empty_slots);

        sleep(2);
    }
    return NULL;
}

int main() {
    pthread_t prod, cons;

    printf("---- Practical 2: Producer-Consumer Test ----\n");
    printf("Buffer Size: %d | Total Items: %d\n\n", BUFFER_SIZE, ITEMS_TO_PRODUCE);

    //Initialize Semaphores and Mutex.
    sem_init(&empty_slots, 0, BUFFER_SIZE);
    sem_init(&full_slots, 0, 0);
    pthread_mutex_init(&buffer_mutex, NULL);

    pthread_create(&prod, NULL, producer, NULL);
    pthread_create(&cons, NULL, consumer, NULL);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    sem_destroy(&empty_slots);
    sem_destroy(&full_slots);
    pthread_mutex_destroy(&buffer_mutex);

    printf("\nDone.\n");
    return 0;
}
