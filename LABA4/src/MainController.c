
#include "AllFunctions.h"

int main(void) {
    MessageQueue* queue = initSharedMemory();

    int arrProducer[512];
    static int producerCount = 0;

    int arrConsumer[512];
    static int consumerCount = 0; 

    while(1) {
        char c = getchar();
        if(c == '1') {

        pid_t pid = fork();
    
         if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
            } else if (pid == 0) {
                messageProducer(queue);
            } else {
                arrProducer[producerCount] = pid;
                producerCount++;
                continue;
            }
        }
        if(c == '2') {
            for (int i = 0; i < producerCount; i++) {
                kill(arrProducer[i], SIGTERM); 
            } 
            producerCount = 0;
        }
        if(c == '3') {

        pid_t pid = fork();
    
         if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
            } else if (pid == 0) {
                messageConsumer(queue);
            } else {
                arrConsumer[consumerCount] = pid;
                consumerCount++;
                continue;
            }
        }
        if(c == '4') {
           for (int i = 0; i < consumerCount; i++) {
                kill(arrConsumer[i], SIGTERM); 
            }  
            consumerCount = 0;
        }
        if(c == 's') {
            printf("Producters: %d\n", producerCount);
            printf("Consumers: %d\n", consumerCount);
        }
        if(c == 'q') {
            for (int i = 0; i < consumerCount; i++) {
                kill(arrConsumer[i], SIGTERM); 
            }  
             for (int i = 0; i < producerCount; i++) {
                kill(arrProducer[i], SIGTERM); 
            } 
            break;
        }
    }
    cleanupSharedMemory(queue);
    return 0;
}
