#include "message.c"

int main(void) {
    pthread_cond_init(&condvar, NULL);
    queueSize = START_QUEUE_SIZE;
    waitFlag = 0;
     
    MessageQueue* queue = malloc(sizeof(MessageQueue));
   sem_t sem_prod, sem_con;
    sem_init(&sem_prod, 0, MAX_SEM_COUNT);
    sem_init(&sem_con, 0, MAX_SEM_COUNT);
    
 
    static int producerCount = 0;
    static int consumerCount = 0; 

    while(1) {
        char c = getchar();
        if(c == '1') {
            threadProdFlag = 1;
            ThreadArgs* argsProd = malloc(sizeof(ThreadArgs));
            argsProd->queue = queue;
            argsProd->sem = &sem_prod;
            pthread_t thread;
            int result = pthread_create(&thread, NULL, threadMessageProducer, (void*)argsProd);
            if (result != 0) {
                perror("pthread_create");
                exit(EXIT_FAILURE);
            }
            producerCount++;
        }
        if(c == '2') {
            threadProdFlag = 0;
            producerCount = 0;
        }
        if(c == '3') {
            threadConFlag = 1;
            ThreadArgs* argsCon = malloc(sizeof(ThreadArgs));
            argsCon->queue = queue;
            argsCon->sem = &sem_con;
            pthread_t thread;
            int result = pthread_create(&thread, NULL, threadMessageConsumer, (void*)argsCon);
            if (result != 0) {
                perror("pthread_create");
                exit(EXIT_FAILURE);
            }
            consumerCount++;
        }
        if(c == '4') {
            threadConFlag = 0;
            consumerCount = 0;
        }
        if(c == 's') {
            printf("Count of producter threads: %d\n", producerCount);
            printf("Count of consumer threads: %d\n", consumerCount);
            printf("Queue size: %d\n", queueSize); 
        }
        if(c == 'q') {
            threadProdFlag = 0;
            threadConFlag = 0;
            pthread_cond_destroy(&condvar);
            pthread_mutex_destroy(&mutex);
            break;
        }
        if(c == '+') {
            queueSize++;
        }
        if(c == '-') {
            if ((queue->tail + 1) % (queueSize + 1) == queue->head) {
                pop(queue);
            }
            queueSize--;
        }
        if(c == 'w') {
            waitFlag = 1;
        }
        if(c == 'r') {
            waitFlag = 0;
            pthread_cond_broadcast(&condvar);
        }
    }
    free(queue);
    return 0;
}