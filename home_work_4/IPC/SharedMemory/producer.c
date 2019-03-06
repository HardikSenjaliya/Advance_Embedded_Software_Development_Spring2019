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
#include <signal.h>
#include <sys/types.h>
#include <sys/syscall.h>

#define NSEC_PER_SEC			(1000000000)

const char *name = "/mySharedMemory";
const char *semNameP = "semaProducer";
const char *semNameC = "semaConsumer";

/*function prototypes*/
void randomStringGenerator(char *, int stringLength);
int setupSigactionHandler(void);
void signalHandler(int, siginfo_t *, void *);
int setSignalMask(void);

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

/*int setSignalMask(){

 sigset_t signalSet;
 sigemptyset(&signalSet);
 sigaddset(&signalSet, SIGINT);
 //sigaddset(&signalSet, SIGUSR2);

 pthread_sigmask(SIG_BLOCK, &signalSet, NULL);

 return 0;

 }*/

/*@brief: this function is a user defined signal handler
 * */
void signalHandler(int signal, siginfo_t *siginfo, void *ucontext) {

	if (signal == SIGINT) {

		struct timespec currentTime = { 0, 0 };

		FILE *pLogFile = fopen("logfile.txt", "a");
		pLogFile = fopen("logfile.txt", "a");
		if (pLogFile == NULL) {
			perror("ERROR: fopen");
			exit(1);
		}

		clock_gettime(CLOCK_MONOTONIC, &currentTime);
		fprintf(pLogFile, "[%ld]SIGINT signal Received...EXITING...",
				(currentTime.tv_sec * NSEC_PER_SEC) + (currentTime.tv_nsec));
		fclose(pLogFile);

		if (0 > sem_unlink(semNameP))
			perror("unlink issue");
		if (0 > sem_unlink(semNameC))
			perror("unlink issue");

		shm_unlink(name);

		exit(1);
	}

}

int setupSigactionHandler() {

	struct sigaction act;

	memset(&act, 0, sizeof(act));

	act.sa_sigaction = signalHandler;
	act.sa_flags = SA_SIGINFO;

	if ((sigaction(SIGINT, &act, NULL)) < 0) {
		printf("ERROR - Registering USR1\n");
		return 1;
	}

	return 0;
}

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

	/*setup signal action and user defined signal handler to handle
	 * SIGINT signal*/
	setupSigactionHandler();

	struct timespec currentTime = { 0, 0 };

	FILE *pLogFile = fopen("logfile.txt", "a");
	pLogFile = fopen("logfile.txt", "a");
	if (pLogFile == NULL) {
		perror("ERROR: fopen");
		exit(0);
	}

	clock_gettime(CLOCK_MONOTONIC, &currentTime);
	fprintf(pLogFile, "[%ld]IPC - Shared Memory\nProducer Process ID: %d\n",
			(currentTime.tv_sec * NSEC_PER_SEC) + (currentTime.tv_nsec),
			getpid());

	int shmFD;

	void *pSharedMemory;

	/*open a new sharead memory object*/
	shmFD = shm_open(name, O_CREAT | O_RDWR, 0666);
	if (shmFD < 0) {
		perror("ERROR: shm_open");
		exit(1);
	}

	/*truncate the opened shared memory file descriptor upto
	 * the requried size*/
	int size = ftruncate(shmFD, SIZE);
	if (size < 0) {
		perror("ERROR: ftruncate");
		exit(1);
	}

	/*creates a mapping to the virtual address space and such that it is
	 * readable and writeable and shared between processes*/
	pSharedMemory = mmap(0, SIZE, PROT_WRITE | PROT_WRITE, MAP_SHARED, shmFD,
			0);
	if (pSharedMemory < 0) {
		perror("ERROR: mmap");
		exit(1);
	}

	/*removes the semaphores if using by other proccesses*/
	if (0 > sem_unlink(semNameP))
		perror("unlink issue");
	if (0 > sem_unlink(semNameC))
		perror("unlink issue");

	/*Open a semaphore for producer process*/
	sem_t *semaphoreP = sem_open(semNameP, O_CREAT, 0666, 0);
	if (semaphoreP == SEM_FAILED) {
		perror("ERROR: sem_open");
		exit(1);
	}

	/*Open a semaphore a consumer process*/
	sem_t *semaphoreC = sem_open(semNameC, O_CREAT, 0666, 0);
	if (semaphoreC == SEM_FAILED) {
		perror("ERROR: sem_open");
		exit(1);
	}

	/*this loop reads and writes to the shared memory*/
	for (int i = 0; i < 10; i++) {

		/*generate a random string to be written*/
		int size = rand() % 31;
		char message[size];
		randomStringGenerator(message, size);

		processData_t sendingData;

		strcpy(sendingData.string, message);
		sendingData.stringLength = strlen(message);
		sendingData.ledStatus = LED_ON;

		clock_gettime(CLOCK_MONOTONIC, &currentTime);
		fprintf(pLogFile,
				"[%ld]Produucer Sending: String - %s stringLength - %d, LED status - %d\n",
				(currentTime.tv_sec * NSEC_PER_SEC) + (currentTime.tv_nsec),
				sendingData.string, sendingData.stringLength,
				sendingData.ledStatus);

		/*writes data to the shared memory*/
		memcpy((processData_t*) pSharedMemory, &sendingData,
				sizeof(processData_t));

		/*signal the semaphore to read the data by the consumer process*/
		if (0 > sem_post(semaphoreP))
			perror("ERROR: sem_post");

		/*wait here for the signal from the consumer process
		 * indicating data is available to read*/
		if (0 > sem_wait(semaphoreC))
			perror("ERROR: sem_wait");

		processData_t receivedData;

		/*read data from the shared memory*/
		memcpy(&receivedData, (processData_t*) pSharedMemory,
				sizeof(processData_t));

		clock_gettime(CLOCK_MONOTONIC, &currentTime);
		fprintf(pLogFile,
				"[%ld]Produucer Receiving: String - %s stringLength - %d, LED status - %d\n",
				(currentTime.tv_sec * NSEC_PER_SEC) + (currentTime.tv_nsec),
				receivedData.string, receivedData.stringLength,
				receivedData.ledStatus);

		printf(
				"Produucer Received Message string: %s, string length: %d, led status: %d\n",
				receivedData.string, receivedData.stringLength,
				receivedData.ledStatus);
	}

	//printf("Producing Exiting\n");

	return 0;

}
