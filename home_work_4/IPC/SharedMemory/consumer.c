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

/*function prototypes*/
void randomStringGenerator(char *, int stringLength);

/*enum for led status*/
typedef enum {
	LED_OFF, LED_ON
} led;

/*structure for data to be sent*/
typedef struct {

	char string[30];
	int stringLength;
	int ledStatus;
} processData_t;

/*
 * @brief: this function generates random string from the predefined list of characters
 * 			and given size of the string
 * @param randomString: pointer to store generated random string
 * @param stringLength: length of the string to be generated
 * @return void
 *
 * */
void randomStringGenerator(char *randomString, int stringLength) {

	char characters[] = "abcdefghijklmnopqrstuvwxyz"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	while (stringLength-- > 0) {
		int index = (double) rand() / RAND_MAX * (sizeof characters - 1);
		*randomString++ = characters[index];
	}

	*randomString = '\0';

}

int main(void) {

	const int SIZE = 4096;

	struct timespec currentTime = { 0, 0 };

	FILE *pLogFile = fopen("logfile.txt", "a");
	pLogFile = fopen("logfile.txt", "a");
	if (pLogFile == NULL) {
		perror("ERROR: fopen");
		exit(0);
	}

	clock_gettime(CLOCK_MONOTONIC, &currentTime);
	fprintf(pLogFile, "[%ld]IPC - Shared Memory\nServer Process ID: %d\n",
			(currentTime.tv_sec * NSEC_PER_SEC) + (currentTime.tv_nsec),
			getpid());

	int shmFD;

	void *pSharedMemory;

	/*Open a shared memory object created by the producer process*/
	shmFD = shm_open(name, O_CREAT | O_RDWR, 0666);
	if (shmFD < 0) {
		perror("ERROR: shm_open");
		exit(1);
	}

	/*map the memory to virtual address space and make it readbale and writable for
	 * other processes*/
	pSharedMemory = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmFD, 0);
	if (pSharedMemory < 0) {
		perror("ERROR: mmap");
		exit(1);
	}

	/*open a named semaphore for a producer process*/
	sem_t *semaphoreP = sem_open(semNameP, 0);
	if (semaphoreP == SEM_FAILED) {
		perror("ERROR: sem_open producer");
		exit(1);
	}

	/*open a named semaphore for a consumer process*/
	sem_t *semaphoreC = sem_open(semNameC, 0);
	if (semaphoreC == SEM_FAILED) {
		perror("ERROR: sem_open consumer");
		exit(1);
	}

	/*wait here till the producer writes data to the shared memory*/
	if (0 > sem_wait(semaphoreP))
		perror("ERROR: sem_wait");

	/*This loop reads data from the memory and writes to the memroy*/
	for (int i = 0; i < 10; i++) {

		processData_t receivedData = { 0 };

		char *streamData = (char*) &receivedData;

		/*read data from the memory*/
		memcpy(streamData, (char*) pSharedMemory, sizeof(processData_t));

		clock_gettime(CLOCK_MONOTONIC, &currentTime);
		fprintf(pLogFile,
				"[%ld]Consumer Receiving: String - %s stringLength - %d, LED status - %d\n",
				(currentTime.tv_sec * NSEC_PER_SEC) + (currentTime.tv_nsec),
				receivedData.string, receivedData.stringLength,
				receivedData.ledStatus);

		printf(
				"Consumer Received Data string: %s, string length: %d, led status: %d\n",
				receivedData.string, receivedData.stringLength,
				receivedData.ledStatus);

		processData_t sendingData;

		/*generate a random string to sent*/
		int size = rand() % 31;
		char message[size];
		randomStringGenerator(message, size);

		strcpy(sendingData.string, message);
		sendingData.stringLength = strlen(message);
		sendingData.ledStatus = LED_ON;

		clock_gettime(CLOCK_MONOTONIC, &currentTime);
		fprintf(pLogFile,
				"[%ld]Consumer Sending: String - %s stringLength - %d, LED status - %d\n",
				(currentTime.tv_sec * NSEC_PER_SEC) + (currentTime.tv_nsec),
				sendingData.string, sendingData.stringLength,
				sendingData.ledStatus);

		/*write message to the shared memory*/
		memcpy((processData_t*) pSharedMemory, &sendingData,
				sizeof(processData_t));

		/*signal the producer process that the data is available to read*/
		if (0 > sem_post(semaphoreC))
			perror("ERROR: sem_post consumer");

	}

	/*unlink the semaphores*/
	if (-1 == sem_unlink(semNameP))
		perror("ERROR: sem_unlink");

	if (-1 == sem_unlink(semNameC))
		perror("ERROR: sem_unlink");

	/*unlink the shared memory*/
	shm_unlink(name);

	return 0;
}
