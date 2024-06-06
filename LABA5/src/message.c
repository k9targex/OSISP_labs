#include "queue.c"
pthread_cond_t condvar = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void messageProducer(MessageQueue *queue, sem_t *sem) {

    while (1) {
        if(waitFlag) {
            pthread_mutex_lock(&mutex);
            pthread_cond_wait(&condvar, &mutex);
            pthread_mutex_unlock(&mutex);
        }
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
        if(!threadProdFlag) {
            break;
        }
    }
}

void messageConsumer(MessageQueue *queue, sem_t *sem) {
    while (1) {
        if(waitFlag) {
            pthread_mutex_lock(&mutex);
            pthread_cond_wait(&condvar, &mutex);
            pthread_mutex_unlock(&mutex);
        }
        sem_wait(sem);
        pop(queue);
        sleep(1);
        sem_post(sem);
        if(!threadConFlag) {
            break;
        }
    }
}