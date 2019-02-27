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

void childProcess() {
	struct timespec currentTime = { 0, 0 };

	pthread_mutex_lock(&mLogFile);
	pLogFile = fopen("logfile.txt", "a");
	if (pLogFile == NULL) {
		perror("ERROR: fopen");
		exit(0);
	}
	clock_gettime(CLOCK_MONOTONIC, &currentTime);
	fprintf(pLogFile,
			"[%ld]IPC - PIPE\nChild Process ID: %d\nAllocated File Descriptor: pipe_c2p\n",
			(currentTime.tv_sec*NSEC_PER_SEC) + (currentTime.tv_nsec),getpid());
	fclose(pLogFile);
	pthread_mutex_unlock(&mLogFile);

	/*Read from the Parent process*/

	close(pipe_p2c[1]); /*close file descriptor for writing to parent process*/

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
			"[%ld]Received String:%s\nMessage Length:%d\nLED status:%d\n",(currentTime.tv_sec*NSEC_PER_SEC) + (currentTime.tv_nsec)
			,pData->string, pData->stringLength, pData->ledStatus);
	fclose(pLogFile);
	pthread_mutex_unlock(&mLogFile);

	close(pipe_p2c[0]);

	/*Send message to Parent Process*/

}

void parentProcess() {

	struct timespec currentTime = { 0, 0 };

	pthread_mutex_lock(&mLogFile);
	pLogFile = fopen("logfile.txt", "a");
	if (pLogFile == NULL) {
		perror("ERROR: fopen");
		exit(0);
	}
	clock_gettime(CLOCK_MONOTONIC, &currentTime);
	fprintf(pLogFile,
			"[%ld] IPC - PIPE\nParent Process ID: %d\nAllocated File Descriptor: pipe_p2c\n",
			(currentTime.tv_sec * NSEC_PER_SEC) + (currentTime.tv_nsec),
			getpid());
	fclose(pLogFile);
	pthread_mutex_unlock(&mLogFile);

	/*Write to the child Process*/

	close(pipe_p2c[0]);

	processData_t pData;

	char writeMsg[] = "Hello from the Parent Process";
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
	fprintf(pLogFile, "[%ld] Sending String: %s, LED status:%d\n",
			(currentTime.tv_sec * NSEC_PER_SEC) + (currentTime.tv_nsec),
			pData.string, pData.ledStatus);
	fclose(pLogFile);
	pthread_mutex_unlock(&mLogFile);

	write(pipe_p2c[1], &pData, sizeof(pData));

	close(pipe_p2c[1]);

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
