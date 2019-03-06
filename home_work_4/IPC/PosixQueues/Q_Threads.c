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

/*function prototypes*/
void *threadFun1(void *params);
void *threadFun2(void *params);
void randomStringGenerator(char *, int stringLength);

/*enum for led status*/
typedef enum {
	LED_OFF, LED_ON
} led;

/*structure of the data to be sent*/
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

FILE *pLogFile;

/*
 * @brief: this is the thread function for thread 1
 * it creates a POSIX message queue for sending and receiving messages
 *
 * */
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

	/*set the attributes of the POSIX queue
	 * max number of messages are set to default
	 * and size of the message is equal to the size of the structure which
	 * contains the data to be sent*/

	struct mq_attr attr;
	char buffer[MAX_SIZE];

	attr.mq_flags = 0;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = sizeof(processData_t);
	attr.mq_curmsgs = 0;

	/*open a queue*/
	myQDes = mq_open(QUEUE_NAME, O_RDWR | O_NONBLOCK | O_CREAT, 0666, &attr);

	clock_gettime(CLOCK_MONOTONIC, &currentTime);
	fprintf(pLogFile,
			"[%ld]IPC - PosixQueues\nThread1 Process ID: %ld\nAllocated File Descriptor: %d\n",
			(currentTime.tv_sec * NSEC_PER_SEC) + (currentTime.tv_nsec),
			pthread_self(), myQDes);

	//memset(buffer, 0x00, sizeof(buffer));

	processData_t receivedData;

	/*This loop reads data ten times and logs it to the logfile*/
	while (count) {

		/*read the data*/
		bytesRead = mq_receive(myQDes, (char*) &receivedData,
				sizeof(receivedData), &sender);
		if (bytesRead >= 0) {
			/*printf("Thread1: Received message: %s Message Length: %d LED status: %d\n",
			 receivedData.string, receivedData.stringLen, receivedData.ledStatus);
			 */
			fprintf(pLogFile,
					"[%ld] Thread1 Receiving : String - %s stringLength - %d, LED status - %d\n",
					(currentTime.tv_sec * NSEC_PER_SEC) + (currentTime.tv_nsec),
					receivedData.string, receivedData.stringLength,
					receivedData.ledStatus);

		} else {
			perror("ERROR: mq_receive");
		}

		sleep(1);

		count--;
	}

	count = 10;

	processData_t sendingData;

	/*this loop sends data ten times and logs it to the logfile*/
	while (count) {

		/*Generate a random string to be sent*/
		int size = rand() % 31;
		char dataString[size];
		randomStringGenerator(dataString, size);

		strcpy(sendingData.string, dataString);
		sendingData.stringLength = strlen(dataString);
		sendingData.ledStatus = LED_OFF;

		printf("Thread1: Send message... \n");
		fprintf(pLogFile,
				"[%ld] Thread1 Sending: String - %s stringLength - %d, LED status - %d\n",
				(currentTime.tv_sec * NSEC_PER_SEC) + (currentTime.tv_nsec),
				sendingData.string, sendingData.stringLength,
				sendingData.ledStatus);

		/*send the data*/
		int bytesRead = mq_send(myQDes, (const char*) &sendingData,
				sizeof(sendingData), 0);
		if (bytesRead < 0)
			perror("ERROR : mq_send");
		// printf("Thread2: send %d %d \n", errno, bytesRead);

		sleep(1);

		count--;
	}

	if (-1 == mq_close(myQDes))
		perror("ERROR: mq_close");
	if (-1 == mq_unlink(QUEUE_NAME))
		perror("ERROR: mq_unlink");

	return NULL;
}

/*
 * @brief: this is the thread function for thread 1
 * it creates a POSIX message queue for sending and receiving messages
 *
 * */
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

	/*set the attributes of the POSIX queue
	 * max number of messages are set to default
	 * and size of the message is equal to the size of the structure which
	 * contains the data to be sent*/
	struct mq_attr attr;

	attr.mq_flags = 0;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = sizeof(processData_t);
	attr.mq_curmsgs = 0;

	/*Open the queue*/
	myQDes = mq_open(QUEUE_NAME, O_CREAT | O_RDWR | O_NONBLOCK, 0666, &attr);

	clock_gettime(CLOCK_MONOTONIC, &currentTime);
	fprintf(pLogFile,
			"[%ld]IPC - PosixQueues\nThread2 Process ID: %ld\nAllocated File Descriptor: %d\n",
			(currentTime.tv_sec * NSEC_PER_SEC) + (currentTime.tv_nsec),
			pthread_self(), myQDes);

	processData_t sendingData;

	/*this loop sends data ten tome and logs it to the logfile*/
	while (count) {

		/*Generate a random string to sent*/
		int size = rand() % 31;
		char dataString[size];
		randomStringGenerator(dataString, size);

		strcpy(sendingData.string, dataString);
		sendingData.stringLength = strlen(dataString);
		sendingData.ledStatus = LED_ON;

		printf("Thread2: Send message... \n");
		fprintf(pLogFile,
				"[%ld] Thread2 Sending: String - %s stringLength - %d, LED status - %d\n",
				(currentTime.tv_sec * NSEC_PER_SEC) + (currentTime.tv_nsec),
				sendingData.string, sendingData.stringLength,
				sendingData.ledStatus);

		/*send data*/
		int bytesRead = mq_send(myQDes, (const char*) &sendingData,
				sizeof(sendingData), 0);
		if (bytesRead < 0)
			perror("ERROR : mq_send");
		// printf("Thread2: send %d %d \n", errno, bytesRead);

		sleep(1);

		count--;
	}

	count = 10;

	processData_t receivedData;

	/*this loop reads data ten times and logs it to the logfile*/
	while (count) {

		/*read the data*/
		bytesRead = mq_receive(myQDes, (char*) &receivedData,
				sizeof(receivedData), &sender);
		if (bytesRead >= 0) {
			/*        printf("Thread1: Received message: %s Message Length: %d LED status: %d\n",
			 receivedData.string, receivedData.stringLen, receivedData.ledStatus);*/
			fprintf(pLogFile,
					"[%ld] Thread2 Receiving: String - %s stringLength - %d, LED status - %d\n",
					(currentTime.tv_sec * NSEC_PER_SEC) + (currentTime.tv_nsec),
					receivedData.string, receivedData.stringLength,
					receivedData.ledStatus);
		} else {
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

	/*create two threads*/
	if (pthread_create(&thread1, NULL, &threadFun1, NULL))
		printf("Error: creating thread1");

	if (pthread_create(&thread2, NULL, &threadFun2, NULL))
		printf("Error: creating thread2");

	if (pthread_join(thread1, NULL))
		printf("Error: joining thread1");
	if (pthread_join(thread2, NULL))
		printf("Error: joining thread1");

	return 0;
}
