#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#define ARRAY_SIZE				256
#define MY_MEREGESORT_NUMBER	399
#define GET_PID_SYSCALL			20
#define GET_USER_ID				24
#define GET_TIME				78

int main(void){

	int fromSysCall = 0;
	int32_t array_index = 0;

	time_t ctime = time(NULL);
	struct tm currentTime = *localtime(&ctime);

	int32_t *pUnsortedArray = (int32_t*)malloc(sizeof(int32_t) * ARRAY_SIZE);
	int32_t *pSortedArray = (int32_t*)malloc(sizeof(int32_t) * ARRAY_SIZE);

	/*Current Process ID*/
	long currentPid = syscall(GET_PID_SYSCALL);
	printf("Current Process ID: %ld\n", currentPid);

	/*Current User ID*/
	long currentUserId = syscall(GET_USER_ID);
   	printf("Current User ID: %ld\n", currentUserId);
	
	/*Reference - https://en.cppreference.com/w/c/chrono/tm */ 
   	printf("Current Time HH:MM:SS %d:%d:%d\n", currentTime.tm_hour, currentTime.tm_min, currentTime.tm_sec);
   	printf("Current Date YYYY-MM-DD %d-%d-%d\n", currentTime.tm_year + 1900, currentTime.tm_mon + 1, currentTime.tm_mday);

	printf("*** Unsorted Array ***\n");

	for(array_index = 0; array_index < ARRAY_SIZE; array_index++){
		*(pUnsortedArray + array_index) = rand() % 1000 + 1;
		printf("%d ", *(pUnsortedArray + array_index));
	}

	printf("\n");

	/*Call to the system call my_mergesort*/
	fromSysCall = syscall(MY_MEREGESORT_NUMBER, pUnsortedArray, pSortedArray, ARRAY_SIZE);

	if(fromSysCall == 0){
		printf("*** Sorted Array ***\n");
		for(array_index = 0; array_index < ARRAY_SIZE; array_index++){
			printf("%d ", *(pSortedArray + array_index));
		}

		printf("\n");
	}
	else{
		printf("Error from system call\n");
	}

	/*Added to count the approximate time taken to perform the system call*/
	printf("Current Time HH:MM:SS %d:%d:%d\n", currentTime.tm_hour, currentTime.tm_min, currentTime.tm_sec);
	
	return 0;

}