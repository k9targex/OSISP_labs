#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "AllFunctions.h"
void messageProducer(MessageQueue *queue, sem_t *sem) {
   while (1) {
        sem_wait(sem);  
        Message *new_message = (Message *)malloc(sizeof(Message));
        if (new_message == NULL) {
            perror("Error allocating memory for message");
            exit(EXIT_FAILURE);
        }

        generateRandomMessage(new_message);
        push(queue, new_message); 
        sleep(1);  
        sem_post(sem);  
    }
}

void messageConsumer(MessageQueue *queue, sem_t *sem) {
    while (1) {
        sem_wait(sem);  
        pop(queue);  
        sleep(1);  
        sem_post(sem);  
    }
}
