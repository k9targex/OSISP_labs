#include "thread.c"

void push(MessageQueue *queue, Message *message) {
    pthread_mutex_lock(&queue->mutex); 
    if ((queue->tail + 1) % (queueSize + 1) == queue->head) {
        printf("queue is full!!!\n");
        pthread_mutex_unlock(&queue->mutex); 
        return;
    }

    queue->buffer[queue->tail] = message;
    queue->tail = (queue->tail + 1) % (queueSize + 1);
    queue->count_added++;

    pthread_mutex_unlock(&queue->mutex); 
    printf("Message added to the queue: message=%s, type=%d, hash=%d, size=%d, count added=%d, count extracted=%d\n", message->data, message->type, message->hash, message->size, queue->count_added, queue->count_extracted);
}

Message *pop(MessageQueue *queue) {
    pthread_mutex_lock(&queue->mutex); 

    if (queue->head == queue->tail) {

        printf("queue is empty!!!\n");
        pthread_mutex_unlock(&queue->mutex); 
        return NULL;
    }

    Message *message = queue->buffer[queue->head];
    queue->head = (queue->head + 1) % (queueSize + 1);
    queue->count_extracted++;

    pthread_mutex_unlock(&queue->mutex); 
    printf("Message deleted to the queue: message=%s, type=%d, hash=%d, count added=%d, count extracted=%d\n", message->data, message->type, message->hash, queue->count_added, queue->count_extracted);
    return message;
}

void generateRandomMessage(Message *message) {


    message->type = rand() % 256;

    message->hash = hashCode(message);

    for (int i = 0; i < 10; i++) {
       message->data[i] = 'a' + rand() % 26;
    }
}


uint16_t hashCode(const Message *message) {
    uint16_t sum1 = 0;
    uint16_t sum2 = 0;

    sum1 = (sum1 + message->type) % 255;
    sum2 = (sum2 + sum1) % 255;

    sum1 = (sum1 + message->size) % 255;
    sum2 = (sum2 + sum1) % 255;

    for (int i = 0; i < 10; i++) {
        sum1 = (sum1 + message->data[i]) % 255;
        sum2 = (sum2 + sum1) % 255;
    }

    return (sum2 << 8) | sum1;
}