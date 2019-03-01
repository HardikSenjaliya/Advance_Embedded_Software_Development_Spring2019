/*
 * pipe.c
 *
 *  Created on: Feb 26, 2019
 *      Author: hardyk
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>

#define NSEC_PER_SEC				(1000000000)
#define STRING_LENGTH				(20)
#define PIPE_READ_END				(0)
#define PIPE_WRITE_END				(1)

/*Function prototypes*/
void childProcess(void);
void parentProcess(void);

void randomStringGenerator(char *, int stringLength);

/*Global variables*/

/*declare pipe file descriptors
 * pipe 1 - p2c parent to child
 * pipe 2 - c2p child to parent
 * two pipes are used for bidirectional communication
 * */
int pipe_p2c[2], pipe_c2p[2];

/*file pointer for log file*/
FILE *pLogFile;

/*Mutex for log file read and write*/
pthread_mutex_t mLogFile;

/*Enum led status on or off*/
typedef enum {

	LED_OFF, LED_ON
} led;

/*data to be sent between processes*/
typedef struct {

	char string[30];
	int stringLength;
	int ledStatus;

} processData_t;

void randomStringGenerator(char *randomString, int stringLength){

	 char characters[] = "abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (stringLength-- > 0) {
        int index = (double) rand() / RAND_MAX * (sizeof characters - 1);
        *randomString++ = characters[index];
    }

    *randomString = '\0';

}

	
void childProcess() {
	struct timespec currentTime = { 0, 0 };

	int count = 10;

	pthread_mutex_lock(&mLogFile);
	pLogFile = fopen("logfile.txt", "a");
	if (pLogFile == NULL) {
		perror("ERROR: fopen");
		exit(0);
	}
	clock_gettime(CLOCK_MONOTONIC, &currentTime);
	fprintf(pLogFile,
			"[%ld]IPC - PIPE Child Process ID: %d Allocated File Descriptor: pipe_c2p \n",
			(currentTime.tv_sec*NSEC_PER_SEC) + (currentTime.tv_nsec),getpid());
	fclose(pLogFile);
	pthread_mutex_unlock(&mLogFile);

	/*Read from the Parent process*/

	//close(pipe_p2c[1]); /*close file descriptor for writing to parent process*/

	for(int i=0; i<10; i++){

		char readMsg[sizeof(processData_t)] = { 0 };
		read(pipe_p2c[0], readMsg, sizeof(processData_t));

		processData_t *pData = (processData_t*) readMsg;

		pthread_mutex_lock(&mLogFile);
		pLogFile = fopen("logfile.txt", "a");
		if (pLogFile == NULL) {
		perror("ERROR: fopen");
			exit(0);
		}

		clock_gettime(CLOCK_MONOTONIC, &currentTime);
		fprintf(pLogFile,
			"[%ld]Child Process Received String: %s Message Length:%d LED status:%d\n",(currentTime.tv_sec*NSEC_PER_SEC) + (currentTime.tv_nsec)
			,pData->string, pData->stringLength, pData->ledStatus);
	
		fclose(pLogFile);
		pthread_mutex_unlock(&mLogFile);
	}
	//close(pipe_p2c[0]);

	count = 10;

	/*Send message to Parent Process*/
	processData_t sendingData;

	do{
		int size = rand() % 31;
		char writeMsg[size]; 
		randomStringGenerator(writeMsg, size);

		strcpy(sendingData.string, writeMsg);
		sendingData.stringLength = strlen(writeMsg);
		sendingData.ledStatus = LED_OFF;

		pthread_mutex_lock(&mLogFile);
		pLogFile = fopen("logfile.txt", "a");
		if (pLogFile == NULL) {
			perror("ERROR: fopen");
			exit(0);
		}
		clock_gettime(CLOCK_MONOTONIC, &currentTime);
		fprintf(pLogFile, "[%ld]Child Process Sending String: %s, String Length: %d LED status:%d\n",
			(currentTime.tv_sec * NSEC_PER_SEC) + (currentTime.tv_nsec),
			sendingData.string, sendingData.stringLength, sendingData.ledStatus);
		fclose(pLogFile);
		pthread_mutex_unlock(&mLogFile);

		write(pipe_p2c[1], &sendingData, sizeof(sendingData));

		count--;

	}while(count);

}

void parentProcess() {

	struct timespec currentTime = { 0, 0 };
	
	int count = 10;

	pthread_mutex_lock(&mLogFile);
	pLogFile = fopen("logfile.txt", "a");
	if (pLogFile == NULL) {
		perror("ERROR: fopen");
		exit(0);
	}
	clock_gettime(CLOCK_MONOTONIC, &currentTime);
	fprintf(pLogFile,
			"[%ld]IPC - PIPE Parent Process ID: %d Allocated File Descriptor: pipe_p2c\n",
			(currentTime.tv_sec * NSEC_PER_SEC) + (currentTime.tv_nsec),
			getpid());
	fclose(pLogFile);
	pthread_mutex_unlock(&mLogFile);

	/*Write to the child Process*/

	//close(pipe_p2c[0]);

	processData_t pData;

	for(int i=0; i < 10; i++){
		int size = rand() % 31;
		char writeMsg[size]; 
		randomStringGenerator(writeMsg, size);

		strcpy(pData.string, writeMsg);
		pData.stringLength = strlen(writeMsg);
		pData.ledStatus = LED_OFF;

		pthread_mutex_lock(&mLogFile);
		pLogFile = fopen("logfile.txt", "a");
		if (pLogFile == NULL) {
			perror("ERROR: fopen");
			exit(0);
		}
		clock_gettime(CLOCK_MONOTONIC, &currentTime);
		fprintf(pLogFile, "[%ld]Parent Process Sending String: %s, String Length: %d LED status:%d\n",
			(currentTime.tv_sec * NSEC_PER_SEC) + (currentTime.tv_nsec),
			pData.string, pData.stringLength, pData.ledStatus);
		fclose(pLogFile);
		pthread_mutex_unlock(&mLogFile);

		write(pipe_p2c[1], &pData, sizeof(pData));
		sleep(1);

	}
	//close(pipe_p2c[1]);
	count = 10;

	do{
		char readMsg[sizeof(processData_t)] = { 0 };
		read(pipe_p2c[0], readMsg, sizeof(processData_t));

		processData_t *pData = (processData_t*) readMsg;

		pthread_mutex_lock(&mLogFile);
		pLogFile = fopen("logfile.txt", "a");
		if (pLogFile == NULL) {
		perror("ERROR: fopen");
			exit(0);
		}

		clock_gettime(CLOCK_MONOTONIC, &currentTime);
		fprintf(pLogFile,
			"[%ld]Parent Process Received String: %s Message Length:%d LED status:%d\n",(currentTime.tv_sec*NSEC_PER_SEC) + (currentTime.tv_nsec)
			,pData->string, pData->stringLength, pData->ledStatus);
		
		fclose(pLogFile);
		pthread_mutex_unlock(&mLogFile);
		count--;

	}while(count);

}

int main(void) {

	/*Initialize mutex variables*/
	pthread_mutex_init(&mLogFile, NULL);

	/*stores PID of the child*/
	pid_t cpId;

	/*create two pipes*/
	if (pipe(pipe_p2c) == -1) {
		perror("ERROR: pipe");
		exit(0);
	}

	if (pipe(pipe_c2p) == -1) {
		perror("ERROR: pipe");
		exit(0);
	}

	cpId = fork();

	if (cpId == -1) {
		perror("ERROR: fork");
		exit(0);
	}

	/*if cpId = 0 then its a child process else its parent*/
	if (cpId == 0) {
		childProcess();
	} else {
		parentProcess();
	}

	return 0;

}
