#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <time.h>

#define NSEC_PER_SEC			(1000000000)

const char *name = "/mySharedMemory";
const char *semNameP = "semaProducer";
const char *semNameC = "semaConsumer";

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


int main(void){

	const int SIZE = 4096;

	struct timespec currentTime = { 0, 0 };

	FILE *pLogFile = fopen("logfile.txt", "a");
	pLogFile = fopen("logfile.txt", "a");
	if (pLogFile == NULL) {
		perror("ERROR: fopen");
		exit(0);
	}

	clock_gettime(CLOCK_MONOTONIC, &currentTime);
	fprintf(pLogFile,
			"[%ld]IPC - Shared Memory\nProducer Process ID: %d\n",
			(currentTime.tv_sec*NSEC_PER_SEC) + (currentTime.tv_nsec),getpid());

	int shmFD;

	void *pSharedMemory;

	shmFD = shm_open(name, O_CREAT | O_RDWR, 0666);
	if(shmFD < 0){
		perror("ERROR: shm_open");
		exit(1);
	}

	int size = ftruncate(shmFD, SIZE);
	if(size < 0){
		perror("ERROR: ftruncate");
		exit(1);
	}

	pSharedMemory = mmap(0, SIZE, PROT_WRITE | PROT_WRITE, MAP_SHARED, shmFD, 0);
	if(pSharedMemory < 0){
		perror("ERROR: mmap");
		exit(1);
	}

	if (0 > sem_unlink(semNameP)) 
		perror("unlink issue");
	if (0 > sem_unlink(semNameC))
		perror("unlink issue");


	sem_t *semaphoreP = sem_open(semNameP, O_CREAT, 0666, 0);
	if(semaphoreP == SEM_FAILED){
		perror("ERROR: sem_open");
		exit(1);
	}

	sem_t *semaphoreC = sem_open(semNameC, O_CREAT, 0666, 0);
	if(semaphoreC == SEM_FAILED){
		perror("ERROR: sem_open");
		exit(1);
	}


	for(int i = 0; i < 10; i++){

	int size = rand() % 31;
	char message[size];
	randomStringGenerator(message, size);

	processData_t sendingData;

	strcpy(sendingData.string, message);
	sendingData.stringLength = strlen(message);
	sendingData.ledStatus = LED_ON;

	clock_gettime(CLOCK_MONOTONIC, &currentTime);
	fprintf(pLogFile, "[%ld]Produucer Sending: String - %s stringLength - %d, LED status - %d\n", 
			(currentTime.tv_sec*NSEC_PER_SEC) + (currentTime.tv_nsec), sendingData.string, sendingData.stringLength, sendingData.ledStatus);

	memcpy((processData_t*)pSharedMemory, &sendingData, sizeof(processData_t));

	if(0 > sem_post(semaphoreP))
		perror("ERROR: sem_post");

	if(0 > sem_wait(semaphoreC))
		perror("ERROR: sem_wait");

	processData_t receivedData;

	memcpy(&receivedData, (processData_t*)pSharedMemory, sizeof(processData_t));

	clock_gettime(CLOCK_MONOTONIC, &currentTime);
	fprintf(pLogFile, "[%ld]Produucer Receiving: String - %s stringLength - %d, LED status - %d\n", 
			(currentTime.tv_sec*NSEC_PER_SEC) + (currentTime.tv_nsec), receivedData.string, receivedData.stringLength, receivedData.ledStatus);
	
	printf("Produucer Received Message string: %s, string length: %d, led status: %d\n", receivedData.string, 
		receivedData.stringLength, receivedData.ledStatus);
	}

	//printf("Producing Exiting\n");

	return 0;

}
