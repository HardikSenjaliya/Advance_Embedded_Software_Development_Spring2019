#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/syscall.h>

#define	NUMBER_OF_THREADS 					(2)
#define SIZE_ROW							(256)
#define SIZE_COL							(2)

/*Function prototype*/
void readMapChars(FILE *fp, int array[SIZE_ROW][SIZE_COL]);

pthread_mutex_t mFile;

typedef struct threadData
{
	char *fileName;
	int threadId;
}threadData_t;

FILE *pLogFile;

void readMapChars(FILE *fp, int array[SIZE_ROW][SIZE_COL]){

	char c; 
	while((c = fgetc(fp)) != EOF){
		if((c >= 65) || (c <= 90))
			c = tolower(c);

		array[(int)c][0] = (int)c;
		array[(int)c][1] += 1;
	}
}



void *funThread0(void *arg){
	
	threadData_t *data = (threadData_t*)(arg);
	int mapChars[SIZE_ROW][SIZE_COL];
	int indexR = 0, indexC = 0;

	/*Initialize the array to 0*/
	for(indexR = 0; indexR < SIZE_ROW; indexR++){
		for(indexC = 0; indexC < SIZE_COL; indexC++)
			mapChars[indexR][indexC] = 0;
	}

	/*Read values of posixID and LinuxID of a thread*/
	long int posixID = pthread_self();
	int linuxID = syscall(SYS_gettid);

	printf("thread 0 %ld and %d \n", posixID, linuxID);

	/*Get the exclusive access of a logfile*/
	pthread_mutex_lock(&mFile);

	/*Open the logfile*/
	pLogFile = fopen(data->fileName, "a");
	if(pLogFile == NULL)
		printf("Unable to open log file\n");

	/*Write to the log file*/
	fprintf(pLogFile, "Thread 0 is writing to the file\n");
	fprintf(pLogFile, "POSIX id is %ld and LINUX id is %d\n", posixID, linuxID);

	/*Open the test file to read data*/
	FILE *pTestFile = fopen("gdb.txt", "r");
	if(pTestFile == NULL)
		printf("Unable to open test file\n");

	/*Map all the characters in the file*/
	readMapChars(pTestFile, mapChars);

	/*Close the test file*/
	fclose(pTestFile);

	/*Write all the characters with frequency less than 100 to the logfile*/
	for(indexR = 0; indexR < SIZE_ROW; indexR++){
		//printf("%c and %d\n", (char)mapChars[indexR][0], mapChars[indexR][1]);
		if(mapChars[indexR][1] < 100 && mapChars[indexR][1] > 0)
			fprintf(pLogFile,"%c\t %d\t\n", mapChars[indexR][0], mapChars[indexR][1]);
	}

	fclose(pLogFile);

	pthread_mutex_unlock(&mFile);

	return NULL;
}




void *funThread1(void *arg){

	threadData_t *data = (threadData_t*)(arg);

	long int posixID = pthread_self();
	int linuxID = syscall(SYS_gettid);

	printf("thread 1 %ld and %d \n", posixID, linuxID);

	pthread_mutex_lock(&mFile);
	pLogFile = fopen(data->fileName, "a");
	if(pLogFile == NULL)
		printf("Unable to open file\n");

	fprintf(pLogFile, "Thread 1 is writing to the file\n");
	fprintf(pLogFile, "POSIX id is %ld and LINUX id is %d\n", posixID, linuxID);

	fclose(pLogFile);

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
	if(pthread_create(&threads[0], NULL, funThread0, (void*)&threadData[0]))
		printf("Error while spawning a thread\n");
	sleep(1);
	if(pthread_create(&threads[1], NULL, funThread1, (void*)&threadData[1]))
		printf("Error while spawning a thread\n");
	sleep(1);
	
	/*Main threads waits for two child threads to complete the execution*/
	for(index = 0; index < NUMBER_OF_THREADS; index++){

		pthread_join(threads[index], NULL);
	}

	


/*	FILE *pLogFile = fopen(logFile, "w");
	if(pLogFile == NULL)
		printf("Unable to open a file\n");
*/
	return 0;

}


