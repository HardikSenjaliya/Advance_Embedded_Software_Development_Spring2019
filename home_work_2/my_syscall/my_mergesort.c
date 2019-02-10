#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#define MY_SYSCALL_NUMBER			398
#define MY_MEREGESORT_NUMBER		399

#define ARRAY_SIZE					256

//#define TEST_INVALID_SYS_CALL_NUMBER
//#define TEST_INVALID_ADDRESS
//#define TEST_INVALID_ARRAY_SIZE
//#define TEST_MULTTIPLE_CALLS

int main(void){

	int32_t fromSyscall = 0, array_index = 0;
    int32_t myArraySize = ARRAY_SIZE;

    /*Check for valid array size. Exit if size is invalid*/
    if(myArraySize < 0 || myArraySize < 256){
        printf("USER SPACE: Wrong array size\n");
        exit(0);
    }

    /*Allocate memory to unsorted array if unable to allocate then exit*/
	int32_t *pUnsortedArray = (int32_t*)malloc(sizeof(int32_t) * ARRAY_SIZE);
    if(pUnsortedArray == NULL){
        printf("USER SPACE: Unable to allocate memory to pUnsortedArray\n");
        exit(0);
    }

    /*Allocate memory to sorted array if unable to allocate then exit*/
	int32_t *pSortedArray = (int32_t*)malloc(sizeof(int32_t) * ARRAY_SIZE);
    if(pUnsortedArray == NULL){
        printf("USER SPACE: Unable to allocate memory to pUnsortedArray\n");
        exit(0);
    }

 	printf("*** USER SPACE: Unsorted array ***\n");
    for(int32_t i = 0; i < ARRAY_SIZE; i++)
    {
        *(pUnsortedArray + i) = rand() % 1000 + 1;
        printf("%d ", *(pUnsortedArray + i));
    }


    /*Test system call with different invalid parameters*/
   
    #ifdef TEST_INVALID_SYS_CALL_NUMBER
        /*Calling with invalid system call number*/
        fromSyscall = syscall(405, pUnsortedArray, pSortedArray, myArraySize);
    #endif

    #ifdef TEST_INVALID_ADDRESS
        /*Calling with invalid address of unsorted and sorted array*/
        fromSyscall = syscall(MY_MEREGESORT_NUMBER, 0, 20, myArraySize);
    #endif

    #ifdef TEST_INVALID_ARRAY_SIZE
        /*Calling with invalid array size*/
        fromSyscall = syscall(MY_MEREGESORT_NUMBER, pUnsortedArray, pSortedArray, 20);
    #endif

    #ifdef TEST_MULTTIPLE_CALLS
        /*Call to system all my_mergesort*/
        for(int32_t i = 0; i < 3; i++)
            fromSyscall = syscall(MY_MEREGESORT_NUMBER, pUnsortedArray, pSortedArray, myArraySize);
    #endif

    fromSyscall = syscall(MY_MEREGESORT_NUMBER, pUnsortedArray, pSortedArray, myArraySize);    


    if(fromSyscall == 0){
        printf("\nUSER SPACE: Sucessfully sorted array\n");   
    	printf("*** USER SPACE: Sorted array ***\n");
    	for(int32_t i = 0; i < ARRAY_SIZE; i++)
    	{
    	    printf("%d ", *(pSortedArray + i));
 	    }
	}
	else
	{
        printf("USER SPACE: Array is not sorted\n");
		printf("USER SAPCE: Error from system call\n ERRRO NUMBER :%x\n return value : %d\n", -EFAULT, fromSyscall);
	}

    /*free allocated memory to unsorted and sorted array*/
    free(pUnsortedArray);
    free(pSortedArray);

    return 0;

}
