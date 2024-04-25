#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "AllFunctions.h"

void messageProducer(void *arg) {
    MessageQueue *queue = (MessageQueue *)arg;
    sem_t semaphore;
    sem_init(&semaphore, 0, MAX_SEM_COUNT);
    
    while (1) {
        sem_wait(&semaphore);
        sem_post(&semaphore);
        
        Message *new_message = (Message *)malloc(sizeof(Message));
        if (new_message == NULL) {
            perror("Error allocating memory for message");
            exit(EXIT_FAILURE);
        }
        
        generateRandomMessage(new_message);
        push(queue, new_message);
        sleep(1);
    }
    
    sem_destroy(&semaphore);
    pthread_exit(NULL);
}

void messageConsumer(void *arg) {
    MessageQueue *queue = (MessageQueue *)arg;
    sem_t semaphore;
    sem_init(&semaphore, 0, MAX_SEM_COUNT);
    
    while (1) {
        sem_wait(&semaphore);
        sem_post(&semaphore);
        pop(queue);
        sleep(1);
    }
    
    sem_destroy(&semaphore);
    pthread_exit(NULL);
}
