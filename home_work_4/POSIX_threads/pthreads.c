#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/syscall.h>

#define	NUMBER_OF_THREADS 					(2)

pthread_mutex_t mFile;

typedef struct threadData
{
	char *fileName;
	int threadId;
}threadData_t;

FILE *pFile;

void *funThread0(void *arg){
	
	threadData_t *data = (threadData_t*)(arg);
	
	long int posixID = pthread_self();
	int linuxID = syscall(SYS_gettid);

	printf("thread 0 %ld and %d \n", posixID, linuxID);

	pthread_mutex_lock(&mFile);

	pFile = fopen(data->fileName, "a");
	if(pFile == NULL)
		printf("Unable to open file\n");

	fprintf(pFile, "Thread 0 is writing to the file\n");
	fprintf(pFile, "POSIX id is %ld and LINUX id is %d\n", posixID, linuxID);

	fclose(pFile);

	pthread_mutex_unlock(&mFile);

	return NULL;

}


void *funThread1(void *arg){

	threadData_t *data = (threadData_t*)(arg);

	long int posixID = pthread_self();
	int linuxID = syscall(SYS_gettid);

	printf("thread 1 %ld and %d \n", posixID, linuxID);

	pthread_mutex_lock(&mFile);
	pFile = fopen(data->fileName, "a");
	if(pFile == NULL)
		printf("Unable to open file\n");

	fprintf(pFile, "Thread 1 is writing to the file\n");
	fprintf(pFile, "POSIX id is %ld and LINUX id is %d\n", posixID, linuxID);

	fclose(pFile);

	pthread_mutex_unlock(&mFile);

	return NULL;
}


int main(int argc, char **argv){

	threadData_t threadData[NUMBER_OF_THREADS];
	pthread_t threads[NUMBER_OF_THREADS];

	pthread_mutex_init(&mFile, NULL);

	int index = 0;

	/*Error if file name is not provided at the command line*/
	if(argc < 2 )
		printf("Invalid command line argument/No command line argument\nUsage : make run fileName=yourfilename.txt \n");

	/*store the file name from the command line*/
	char *logFile = argv[1];

	/*Create two threads data*/
	for(index = 0; index < NUMBER_OF_THREADS; ++index){

		threadData[index].fileName = logFile;
		threadData[index].threadId = index;

		//printf("%d and %s\n", threadData[index].threadId, threadData[index].fileName);
	}

	/*Create two threads*/
	pthread_create(&threads[0], NULL, funThread0, (void*)&threadData[0]);
	sleep(1);
	pthread_create(&threads[1], NULL, funThread1, (void*)&threadData[1]);
	sleep(1);
	
	/*Main threads waits for two child threads to complete the execution*/
	for(index = 0; index < NUMBER_OF_THREADS; index++){

		pthread_join(threads[index], NULL);
	}

	


/*	FILE *pFile = fopen(logFile, "w");
	if(pFile == NULL)
		printf("Unable to open a file\n");
*/
	return 0;

}


