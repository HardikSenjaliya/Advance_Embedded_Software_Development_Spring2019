#include <fcntl.h>
#include <mqueue.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#define QUEUE_NAME              "/myMessageQ"
#define MAX_SIZE                (1024)
#define NSEC_PER_SEC            (1000000000)

void *threadFun1(void *params);
void *threadFun2(void *params);
void randomStringGenerator(char *, int stringLength);

typedef enum{
    LED_OFF,
    LED_ON
}led;

typedef struct{

    char string[30];
    int stringLength;
    int ledStatus;

}processData_t;

void randomStringGenerator(char *randomString, int stringLength){

     char characters[] = "abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (stringLength-- > 0) {
        int index = (double) rand() / RAND_MAX * (sizeof characters - 1);
        *randomString++ = characters[index];
    }

    *randomString = '\0';

}


FILE *pLogFile;

 void *threadFun1(void *params) {

    int count = 10;
    
    struct timespec currentTime = { 0, 0 };

    pLogFile = fopen("logfile.txt", "a");
    
    if (pLogFile == NULL) {
        perror("ERROR: fopen");
        exit(0);
    }

    mqd_t myQDes;
    unsigned int sender;
    int bytesRead;

    struct mq_attr attr;
    char buffer[MAX_SIZE];

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(processData_t);
    attr.mq_curmsgs = 0;

    myQDes = mq_open(QUEUE_NAME, O_RDWR | O_NONBLOCK | O_CREAT, 0666, &attr);


    clock_gettime(CLOCK_MONOTONIC, &currentTime);
    fprintf(pLogFile,
            "[%ld]IPC - PosixQueues\nThread1 Process ID: %ld\nAllocated File Descriptor: %d\n",
            (currentTime.tv_sec*NSEC_PER_SEC) + (currentTime.tv_nsec),pthread_self(), myQDes);

    //memset(buffer, 0x00, sizeof(buffer));

    processData_t receivedData;

    while(count) {

        bytesRead = mq_receive(myQDes, (char*)&receivedData, sizeof(receivedData), &sender);
        if(bytesRead >= 0) {
            /*printf("Thread1: Received message: %s Message Length: %d LED status: %d\n", 
                receivedData.string, receivedData.stringLen, receivedData.ledStatus);
*/        
        fprintf(pLogFile, "[%ld] Thread1 Receiving : String - %s stringLength - %d, LED status - %d\n", 
            (currentTime.tv_sec*NSEC_PER_SEC) + (currentTime.tv_nsec), 
            receivedData.string, receivedData.stringLength, receivedData.ledStatus);

        }else {
            perror("ERROR: mq_receive");
        }

        sleep(1);

        count--;
    }

    count = 10; 

    processData_t sendingData;

       while(count) {
        
        int size = rand() % 31;
        char dataString[size]; 
        randomStringGenerator(dataString, size);

        strcpy(sendingData.string, dataString);
        sendingData.stringLength = strlen(dataString);
        sendingData.ledStatus = LED_OFF;

        printf("Thread1: Send message... \n");
        fprintf(pLogFile, "[%ld] Thread1 Sending: String - %s stringLength - %d, LED status - %d\n", 
            (currentTime.tv_sec*NSEC_PER_SEC) + (currentTime.tv_nsec), 
            sendingData.string, sendingData.stringLength, sendingData.ledStatus);

        int bytesRead = mq_send(myQDes, (const char*)&sendingData, sizeof(sendingData), 0);
            if(bytesRead < 0)
                perror("ERROR : mq_send");
           // printf("Thread2: send %d %d \n", errno, bytesRead);

        sleep(1);

        count--;
    }

    if(-1 == mq_close(myQDes))
        perror("ERROR: mq_close");
    if(-1 == mq_unlink(QUEUE_NAME))
        perror("ERROR: mq_unlink");

    return NULL;
}

void *threadFun2(void *params) {

    int count = 10;

    struct timespec currentTime = { 0, 0 };

    pLogFile = fopen("logfile.txt", "a");
    
    if (pLogFile == NULL) {
        perror("ERROR: fopen");
        exit(0);
    }

    mqd_t myQDes;
    char buffer[MAX_SIZE];
    int bytesRead;
    unsigned int sender;


    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(processData_t);
    attr.mq_curmsgs = 0;

    myQDes = mq_open(QUEUE_NAME, O_CREAT|O_RDWR|O_NONBLOCK , 0666, &attr);

    clock_gettime(CLOCK_MONOTONIC, &currentTime);
    fprintf(pLogFile,
            "[%ld]IPC - PosixQueues\nThread2 Process ID: %ld\nAllocated File Descriptor: %d\n",
            (currentTime.tv_sec*NSEC_PER_SEC) + (currentTime.tv_nsec), pthread_self(), myQDes);

    processData_t sendingData;

    while(count) {
        
        int size = rand() % 31;
        char dataString[size]; 
        randomStringGenerator(dataString, size);

        strcpy(sendingData.string, dataString);
        sendingData.stringLength = strlen(dataString);
        sendingData.ledStatus = LED_ON;

        printf("Thread2: Send message... \n");
        fprintf(pLogFile, "[%ld] Thread2 Sending: String - %s stringLength - %d, LED status - %d\n", 
            (currentTime.tv_sec*NSEC_PER_SEC) + (currentTime.tv_nsec), 
            sendingData.string, sendingData.stringLength, sendingData.ledStatus);
        int bytesRead = mq_send(myQDes, (const char*)&sendingData, sizeof(sendingData), 0);
            if(bytesRead < 0)
                perror("ERROR : mq_send");
           // printf("Thread2: send %d %d \n", errno, bytesRead);

        sleep(1);

        count--;
    }

    count = 10;

    processData_t receivedData;

      while(count) {

        bytesRead = mq_receive(myQDes, (char*)&receivedData, sizeof(receivedData), &sender);
        if(bytesRead >= 0) {
    /*        printf("Thread1: Received message: %s Message Length: %d LED status: %d\n", 
                receivedData.string, receivedData.stringLen, receivedData.ledStatus);*/
        fprintf(pLogFile, "[%ld] Thread2 Receiving: String - %s stringLength - %d, LED status - %d\n", 
            (currentTime.tv_sec*NSEC_PER_SEC) + (currentTime.tv_nsec), 
            receivedData.string, receivedData.stringLength, receivedData.ledStatus);
        }else {
            printf("Thread1: None %d %d \n", errno, bytesRead);
            perror("ERROR: mq_receive");
        }

        sleep(1);

        count--;
    }

    return NULL;
}

int main() {

    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, &threadFun1, NULL);
    pthread_create(&thread2, NULL, &threadFun2, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}