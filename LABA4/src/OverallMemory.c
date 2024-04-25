#include "AllFunctions.h"

MessageQueue *initSharedMemory(void) {
    int shm_id = shmget(SHARED_MEMORY_KEY, sizeof(MessageQueue), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    void *shared_memory = shmat(shm_id, NULL, 0);
    if (shared_memory == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    MessageQueue *queue = (MessageQueue *)shared_memory;

    return queue;
}

void cleanupSharedMemory(MessageQueue *queue) {
    if (shmdt(queue) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
    int shm_id = shmget(SHARED_MEMORY_KEY, sizeof(MessageQueue), 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }
}