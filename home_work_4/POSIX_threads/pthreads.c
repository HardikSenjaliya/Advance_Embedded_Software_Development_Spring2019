/*
 * threads.c
 *
 *  Created on: Feb 23, 2019
 *      Author: hardyk
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/syscall.h>

#define	NUMBER_OF_THREADS 					(2)
#define SIZE_ROW							(256)
#define SIZE_COL							(2)
#define MSEC_100							(100)
#define MSEC_TO_NSEC						(1000000)
#define NSEC_TO_SEC							(1000000000)
#define MDT									(-7)

/*Function prototype*/
void readMapChars(FILE *fp, int array[SIZE_ROW][SIZE_COL]);
int createPeriodicThread(void);
void timerHandler(union sigval);
int setupSigactionHandler(void);
void signalHandler(int, siginfo_t *, void *);
int setSignalMask(void);

/*Mutex and semaphore for synchronized operation to write into logfile*/
pthread_mutex_t mFile, mTime;
sem_t sFile;

/*Global varibale to handle USR1 and USR2 signal*/
int usrTwoSignal = 0, usrOneSignal = 0;

/*Structure to pass data for thread function*/
typedef struct threadData {
	char *fileName; /*log file name*/

} threadData_t;

/*global: file pointer for a logfile*/
FILE *pLogFile;

/*global: clock/time related variables*/
time_t currentTime;
struct tm *cTime;

void timerHandler(union sigval arg) {
	/*timer expired, run thread1*/
	sem_post(&sFile);
}

int setSignalMask(){

	sigset_t signalSet;
	sigemptyset(&signalSet);
	sigaddset(&signalSet, SIGUSR1);
	sigaddset(&signalSet, SIGUSR2);

	pthread_sigmask(SIG_BLOCK, &signalSet, NULL);

	return 0;

}


void signalHandler(int signal, siginfo_t *siginfo, void *ucontext){

	if(signal == SIGUSR1){
		usrOneSignal = 1;
	}
	else if(signal == SIGUSR2){
		usrTwoSignal = 1;
	}
	else{
		exit(1);
	}

}

int setupSigactionHandler(){

	struct sigaction act;

	memset(&act, 0, sizeof(act));

	act.sa_sigaction = signalHandler;
	act.sa_flags = SA_SIGINFO;

	if((sigaction(SIGUSR1, &act, NULL)) < 0){
			printf("ERROR - Registering USR1\n");
			return 1;
	}
	if((sigaction(SIGUSR2, &act, NULL)) < 0){
			printf("ERROR - Registering USR2\n");
			return 1;
	}
	return 0;
}

int createPeriodicThread() {

	timer_t idTimer;
	struct itimerspec ts;
	struct sigevent se;

	/*Initialize the segevent structure to cause the
	 * signal to handle the event by creating a new thread*/
	se.sigev_notify = SIGEV_THREAD;
	se.sigev_value.sival_ptr = &idTimer;
	se.sigev_notify_function = timerHandler;
	se.sigev_notify_attributes = NULL;

	/*Initialize the timerspec structure for required
	 * period of 100ms*/
	long long int nanoSec = MSEC_100 * MSEC_TO_NSEC;
	ts.it_value.tv_sec = nanoSec / NSEC_TO_SEC;
	ts.it_value.tv_nsec = nanoSec % NSEC_TO_SEC;
	ts.it_interval.tv_sec = ts.it_value.tv_sec;
	ts.it_interval.tv_nsec = ts.it_value.tv_nsec;

	/*create a new timer*/
	if ((timer_create(CLOCK_MONOTONIC, &se, &idTimer)) == -1){
		printf("ERROR - creating a new timer failed\n");
		return 1;
	}
	/*Set the timer to be periodic at 100msec*/
	if ((timer_settime(idTimer, 0, &ts, 0)) == -1){
		printf("ERROR - setting up a period of the timer\n");
		return 1;
	}

	return 0;
}

void readMapChars(FILE *fp, int array[SIZE_ROW][SIZE_COL]) {

	char c;
	while ((c = fgetc(fp)) != EOF && !feof(fp)) {
		if ((c >= 65) || (c <= 90))
			c = tolower(c);

		array[(int) c][0] = (int) c;
		array[(int) c][1] += 1;
	}
}

void *funThread0(void *arg) {

	/*Set signal mask*/
	if(setSignalMask())
		exit(1);

	/*Get an exclusive access of a global clock/time variabels*/
	pthread_mutex_lock(&mTime);
	time(&currentTime);
	cTime = gmtime(&currentTime);

	threadData_t *data = (threadData_t*) (arg);
	int mapChars[SIZE_ROW][SIZE_COL];
	int indexR = 0, indexC = 0;

	/*Initialize the array to 0*/
	for (indexR = 0; indexR < SIZE_ROW; indexR++) {
		for (indexC = 0; indexC < SIZE_COL; indexC++)
			mapChars[indexR][indexC] = 0;
	}

	/*Read values of posixID and LinuxID of a thread*/
	pthread_t posixID = pthread_self();
	pid_t linuxID = syscall(SYS_gettid);

	printf("thread0 %lu and %d \n", posixID, linuxID);

	/*Get an exclusive access of a logfile*/
	pthread_mutex_lock(&mFile);

	/*Open the logfile*/
	pLogFile = fopen(data->fileName, "a");
	if (pLogFile == NULL)
		printf("Unable to open log file\n");

	/*Write to the log file*/
	fprintf(pLogFile, "Thread0 is writing to the file\n");
	fprintf(pLogFile, "Thread0 Starting time : %02d:%02d:%02d\n",
			(cTime->tm_hour), cTime->tm_min, cTime->tm_sec);
	fprintf(pLogFile, "Thread0 POSIX id is %lu and LINUX id is %d\n", posixID,
			linuxID);

	/*Open the test file to read data*/
	FILE *pTestFile = fopen("/usr/bin/gdb.txt", "r");
	if (pTestFile == NULL)
		printf("Unable to open test file\n");

	/*Map all the characters in the file*/
	readMapChars(pTestFile, mapChars);

	/*Close the test file*/
	fclose(pTestFile);

	/*Write all the characters with frequency less than 100 to the logfile*/
	for (indexR = 0; indexR < SIZE_ROW; indexR++) {
		//printf("%c and %d\n", (char)mapChars[indexR][0], mapChars[indexR][1]);
		if (mapChars[indexR][1] < 100 && mapChars[indexR][1] > 0)
			fprintf(pLogFile, "%c\t %d\t\n", mapChars[indexR][0],
					mapChars[indexR][1]);
	}

	time(&currentTime);
	cTime = gmtime(&currentTime);
	fprintf(pLogFile, "Thread0 Task Completed \n Thread0 Exiting time : %02d:%02d:%02d\n",
			(cTime->tm_hour), cTime->tm_min, cTime->tm_sec);

	fclose(pLogFile);

	pthread_mutex_unlock(&mTime);
	pthread_mutex_unlock(&mFile);

	return NULL;
}

void *funThread1(void *arg) {

	long int cpuStat[10];
	char name[5];

	//printf("Entered DEBUG\n");
	/*Get an exclusive access of a global clock/time variabels*/
	pthread_mutex_lock(&mTime);
	time(&currentTime);
	cTime = gmtime(&currentTime);
	pthread_mutex_unlock(&mTime);
	//printf("released mTime1DEBUG\n");
	threadData_t *data = (threadData_t*) (arg);

	pthread_t posixID = pthread_self();
	pid_t linuxID = syscall(SYS_gettid);

	printf("thread1 %lu and %d \n", posixID, linuxID);

	while (!usrTwoSignal) {

		/*Wait till timer expires*/
		sem_wait(&sFile);
		//printf("Timer expired DEBUG\n");
		/*get the exclusive access to logfile*/
		pthread_mutex_lock(&mFile);
		//printf("aquired mFile DEBUG\n");
		pLogFile = fopen(data->fileName, "a");
		if (pLogFile == NULL)
			printf("Unable to open file\n");

		fprintf(pLogFile, "Thread1 is writing to the file\n");
		fprintf(pLogFile, "Thread1 Starting time : %02d:%02d:%02d\n",
				(cTime->tm_hour), cTime->tm_min, cTime->tm_sec);
		fprintf(pLogFile, "Thread1 POSIX id is %lu and LINUX id is %d\n",
				posixID, linuxID);

		/*open stat file to read cpu usage and log the usage into logfile*/
		FILE *pStatFile = fopen("/proc/stat", "r");
		fscanf(pStatFile, "%s %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld", name,
				&cpuStat[0], &cpuStat[1], &cpuStat[2], &cpuStat[3], &cpuStat[4],
				&cpuStat[5], &cpuStat[6], &cpuStat[7], &cpuStat[8],
				&cpuStat[9]);
		fclose(pStatFile);
		fprintf(pLogFile,
				"CPU Utilization\n%s\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\n",
				name, cpuStat[0], cpuStat[1], cpuStat[2], cpuStat[3],
				cpuStat[4], cpuStat[5], cpuStat[6], cpuStat[7], cpuStat[8],
				cpuStat[9]);

		fclose(pLogFile);
		pthread_mutex_unlock(&mFile);
		//printf("released mFile DEBUG \n");
	}

	/*Signal USR2 received, exit and write to the log file*/
	printf("While 1 completed\n");
	pthread_mutex_lock(&mTime);
	time(&currentTime);
	cTime = gmtime(&currentTime);
	pthread_mutex_lock(&mFile);
	pLogFile = fopen(data->fileName, "a");
	if (pLogFile == NULL)
		printf("Unable to open file\n");
	fprintf(pLogFile, "Thread1 USR2 Signal Received \nThread1 Exiting time : %02d:%02d:%02d\n",
			(cTime->tm_hour), cTime->tm_min, cTime->tm_sec);
	fclose(pLogFile);
	pthread_mutex_unlock(&mTime);
	pthread_mutex_unlock(&mFile);

	return NULL;
}

int main(int argc, char **argv) {

	threadData_t threadData[NUMBER_OF_THREADS];
	pthread_t threads[NUMBER_OF_THREADS];

	/*Initialize mutex and semaphore*/
	if (pthread_mutex_init(&mFile, NULL))
		printf("ERROR - initializing mFile mutex\n");

	if (pthread_mutex_init(&mTime, NULL))
		printf("ERROR - initializing mTime mutex\n");

	if ((sem_init(&sFile, 0, 0)) == -1)
		printf("ERROR - initializing a sFile semaphore\n");

	int index = 0;

	/*Error if file name is not provided at the command line*/
	if (argc < 2)
		printf(
				"Invalid command line argument/No command line argument\nUsage : make run fileName=yourfilename.txt \n");

	/*store the file name from the command line*/
	char *logFile = argv[1];

	/*Create two threads data*/
	for (index = 0; index < NUMBER_OF_THREADS; ++index) {
		threadData[index].fileName = logFile;
	}

	/*Create and set the timer*/
	if(createPeriodicThread())
		exit(1);

	/*Set sigaction to handler USR1 and USR2*/
	if(setupSigactionHandler())
		exit(1);

	/*Create two threads*/
	if (pthread_create(&threads[0], NULL, funThread0, (void*) &threadData[0]))
		printf("ERROR - spawning a thread0\n");

	if (pthread_create(&threads[1], NULL, funThread1, (void*) &threadData[1]))
		printf("ERROR - spawning a thread1\n");

	/*Main threads waits for two child threads to complete the execution*/
	for (index = 0; index < NUMBER_OF_THREADS; index++) {

		if (pthread_join(threads[index], NULL)) {
			printf("ERROR - pthread join\n");
		}
	}

	pthread_mutex_lock(&mFile);
	FILE *pLogFile = fopen(logFile, "a");
	if(pLogFile == NULL)
		printf("Unable to open a file\n");
	fprintf(pLogFile, "Main thread - Exiting\n");
	fclose(pLogFile);
	pthread_mutex_unlock(&mFile);
	return 0;

}

