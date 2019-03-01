/*
 * client.c
 *
 *  Created on: Feb 27, 2019
 *      Author: hardyk
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>
#include <sys/socket.h>

#define SERVER_ADDRESS				"localhost"
#define SERVER_ADDRESS_PORT			2345
#define NSEC_PER_SEC			(1000000000)

void readDatafromServer(int fd);
void sendDatatoServer(int fd);
void randomStringGenerator(char *, int stringLength);

/*logfile*/
FILE *pLogFile;

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

void readDatafromServer(int fd){

	int bytesReceived = 0; int count = 10;

	struct timespec currentTime = { 0, 0 };

	char dataReceived[1024];

	pLogFile = fopen("logfile.txt", "a");
	if (pLogFile == NULL) {
		perror("ERROR: fopen");
		exit(0);
	}

	clock_gettime(CLOCK_MONOTONIC, &currentTime);
	fprintf(pLogFile,
			"[%ld]IPC - Sockets\nClient Process ID: %d\nAllocated File Descriptor: %d\n",
			(currentTime.tv_sec*NSEC_PER_SEC) + (currentTime.tv_nsec),getpid(), fd);

	do{
		bytesReceived = read(fd, dataReceived, sizeof(dataReceived));

		processData_t *data = (processData_t*)(dataReceived);

		clock_gettime(CLOCK_MONOTONIC, &currentTime);

			//printf("Data Received: String %s stringLength %d LED %d\n", data->string, data->stringLength, data->ledStatus);

		fprintf(pLogFile, "[%ld] Client Receiving: String - %s stringLength - %d, LED status - %d\n", 
			(currentTime.tv_sec*NSEC_PER_SEC) + (currentTime.tv_nsec), data->string, data->stringLength, data->ledStatus);

		//printf("Data Received: String %s stringLength %d LED %d\n", data->string, data->stringLength, data->ledStatus);
		
		count--;
	}while(count);
}

void sendDatatoServer(int fd){

	int freq = 10;
		
	struct timespec currentTime = { 0, 0 };

	pLogFile = fopen("logfile.txt", "a");
	if (pLogFile == NULL) {
		perror("ERROR: fopen");
		exit(0);
	}
	clock_gettime(CLOCK_MONOTONIC, &currentTime);
	fprintf(pLogFile,
			"[%ld]IPC - Sockets\nClient Process ID: %d\nAllocated File Descriptor: %d\n",
			(currentTime.tv_sec*NSEC_PER_SEC) + (currentTime.tv_nsec),getpid(), fd);
	
	processData_t data;

	for(int i = 0; i < freq; i++){

		int size = rand() % 31;
		char dataString[size]; 
		randomStringGenerator(dataString, size);

		strcpy(data.string, dataString);
		data.ledStatus = LED_OFF;
		data.stringLength = strlen(dataString);

		clock_gettime(CLOCK_MONOTONIC, &currentTime);
		fprintf(pLogFile, "[%ld] Client Sending: String - %s stringLength - %d, LED status - %d\n", 
			(currentTime.tv_sec*NSEC_PER_SEC) + (currentTime.tv_nsec), data.string, data.stringLength, data.ledStatus);

		if(-1 == (send(fd, (const void*)&data, sizeof(data), 0))){
			perror("ERROR: send");
			exit(1);
		}

		sleep(1);
	}
}

int main(void) {

	int socketFD = 0;

	struct sockaddr_in serverSocketAddr;

	/*Create a new socket
	 * domain - IPv4, type - stream based socket, protocol - 0 a single type of
	 * protocol of family IPv4
	 * */

	if (0 > (socketFD = socket(AF_INET, SOCK_STREAM, 0))) {
		fprintf(stderr, "client failed to create socket\n");
		exit(1);
	}

	/*Converts localhost into 0.0.0.0*/
	struct hostent *he = gethostbyname(SERVER_ADDRESS);
	unsigned long serverAddressNBO = *(unsigned long *) (he->h_addr_list[0]);

	/*Configure server socket address*/
	serverSocketAddr.sin_family = AF_INET;
	serverSocketAddr.sin_port = htons(SERVER_ADDRESS_PORT);
	serverSocketAddr.sin_addr.s_addr = serverAddressNBO;

	/*Connect socket to server*/
	if (0 > connect(socketFD, (struct sockaddr *) &serverSocketAddr,
					sizeof(serverSocketAddr))) {
		perror("ERROR: connect");
		close(socketFD);
		exit(1);
	} else {
		printf("Clinet connected to server\n");
	}

	/*Read data from the server*/
	readDatafromServer(socketFD);

	/*Send data to ther server*/
	sendDatatoServer(socketFD);

	close(socketFD);

	return 0;
}
