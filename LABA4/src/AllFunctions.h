#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdint.h>
#include <signal.h> 
#include <semaphore.h>
#define MAX_QUEUE_SIZE 5 
#define SHARED_MEMORY_KEY 8888
#define MAX_SEM_COUNT 10

typedef struct {
   u_int8_t type;
   u_int16_t hash;
   u_int8_t size;
   u_int8_t data[10];
} Message;

typedef struct {
    Message *buffer[MAX_QUEUE_SIZE]; 
    int head; 
    int tail; 
    int count_added; 
    int count_extracted; 
    pthread_mutex_t mutex; 

} MessageQueue;


void push(MessageQueue *queue, Message *message);

Message *pop(MessageQueue *queue);

void generateRandomMessage(Message* message);

uint16_t hashCode(const Message *message);

sem_t* initSharedMemorySem(int key);
MessageQueue* initSharedMemoryQueue(void);
void cleanupSharedMemory(MessageQueue *queue, sem_t *sem_prod, sem_t *sem_con);
void messageProducer(MessageQueue *queue, sem_t *sem);
void messageConsumer(MessageQueue *queue, sem_t *sem);