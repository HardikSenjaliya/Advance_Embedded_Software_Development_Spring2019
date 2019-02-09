#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define MY_SYSCALL_NUMBER			398
#define MY_MEREGESORT_NUMBER		399

#define ARRAY_SIZE					10

int main(void){

	int fromSyscall = 0, array_index = 0;

	int *pUnsortedArray = (int*)malloc(sizeof(int) * ARRAY_SIZE);
	int *pSortedArray = (int*)malloc(sizeof(int) * ARRAY_SIZE);
 
 	printf("Unsorted array is:");
    for(int i = 0; i < ARRAY_SIZE; i++)
    {
        *(pUnsortedArray + i) = rand();
        printf("%d ", *(pUnsortedArray + i));
    }

    if(syscall(MY_MEREGESORT_NUMBER) == 0){
    	printf("\nSorted array is \n");
    	for(int i = 0; i < ARRAY_SIZE; i++)
    	{
    	    printf("%d ", *(pUnsortedArray + i));
 	    }
	}
	else
	{
		printf("Error from system call\n");
	}

    return 0;

}

