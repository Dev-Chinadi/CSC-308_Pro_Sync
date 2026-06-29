// shared_memory_practical4.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <semaphore.h>

#define MSG_SIZE 256

typedef struct {
    sem_t mutex;
    sem_t full;
    sem_t empty;
    char message[MSG_SIZE];
} shared_data_t;

int main() {
    int shmid = shmget(IPC_PRIVATE, sizeof(shared_data_t), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        return 1;
    }

    shared_data_t* shm = (shared_data_t*)shmat(shmid, NULL, 0);
    if (shm == (void*)-1) {
        perror("shmat");
        return 1;
    }

    sem_init(&shm->mutex, 1, 1);
    sem_init(&shm->full, 1, 0);
    sem_init(&shm->empty, 1, 1);

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        sem_wait(&shm->full);
        sem_wait(&shm->mutex);

        printf("\n[Child Process]  Reading from shared memory...\n");
        printf("Child read: %s\n", shm->message);

        sem_post(&shm->mutex);
        sem_post(&shm->empty);

        shmdt(shm);
        exit(0);
    } else {
        sem_wait(&shm->empty);
        sem_wait(&shm->mutex);

        printf("[Parent Process] Writing to shared memory...\n");
        snprintf(shm->message, MSG_SIZE, "Hello from the Parent process.");
        printf("Parent wrote: %s\n", shm->message);

        sem_post(&shm->mutex);
        sem_post(&shm->full);

        wait(NULL);

        sem_destroy(&shm->mutex);
        sem_destroy(&shm->full);
        sem_destroy(&shm->empty);

        shmdt(shm);
        shmctl(shmid, IPC_RMID, NULL);
    }

    return 0;
}
