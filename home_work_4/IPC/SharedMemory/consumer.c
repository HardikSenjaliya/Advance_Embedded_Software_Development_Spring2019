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

const char *name = "/mySharedMemory";
const char *semNameP = "semaProducer";
const char *semNameC = "semaConsumer";

/*#define semNameP "/semaProducer"
#define semNameC "/semaConsumer"*/

typedef enum{
	LED_OFF,
	LED_ON
}led;

typedef struct{

	char string[30];
	int stringLength;
	int ledStatus;
}processData_t;


int main(void){

	const int SIZE = 4096;

	int shmFD;

	void *pSharedMemory;

	shmFD = shm_open(name, O_CREAT | O_RDWR, 0666);
	if(shmFD < 0){
		perror("ERROR: shm_open");
		exit(1);
	}

	pSharedMemory = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmFD, 0);
	if(pSharedMemory < 0){
		perror("ERROR: mmap");
		exit(1);
	}

	sem_t *semaphoreP = sem_open(semNameP, 0);
	if(semaphoreP == SEM_FAILED){
		perror("ERROR: sem_open producer");
		//exit(1);
	}

	sem_t *semaphoreC = sem_open(semNameC, 0);
	if(semaphoreC == SEM_FAILED){
		perror("ERROR: sem_open consumer");
		//exit(1);
	}
	
	if(0 > sem_wait(semaphoreP))
		perror("ERROR: sem_wait");

	processData_t receivedData = {0};

	char *streamData = (char*)&receivedData;

	memcpy(streamData, (char*)pSharedMemory, sizeof(processData_t));

	printf("Consumer Received Data string: %s, string length: %d, led status: %d\n", receivedData.string, 
		receivedData.stringLength, receivedData.ledStatus);

	processData_t sendingData;

	char *message = "Hello from Consumer Process";

	strcpy(sendingData.string,message);
	sendingData.stringLength = strlen(message);
	sendingData.ledStatus = LED_ON;

	memcpy((processData_t*)pSharedMemory, &sendingData, sizeof(processData_t));

	if(0 > sem_post(semaphoreC))
		perror("ERROR: sem_post consumer");

	if(-1 == sem_unlink(semNameP))
		perror("ERROR: sem_unlink");

	if(-1 == sem_unlink(semNameC))
		perror("ERROR: sem_unlink");

	shm_unlink(name);


	return 0;
}