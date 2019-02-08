#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define MY_SYSCALL_NUMBER			398

int main(void){


	int fromSyscall = 0;

	printf("%d\n", syscall(MY_SYSCALL_NUMBER));

	return 0;

}