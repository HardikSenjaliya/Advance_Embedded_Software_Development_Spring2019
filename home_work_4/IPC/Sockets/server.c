/*
 * sockets.c
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
#include <signal.h>
#include <sys/socket.h>

#define SERVER_PORT				(2345)
#define NSEC_PER_SEC			(1000000000)

/*function prototypes*/
void sendDataToClient(int fd);
void readDataFromClient(int fd);
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

/*@brief: this function is a user defined signal handler
 * */
void signalHandler(int signal, siginfo_t *siginfo, void *ucontext){

	if(signal == SIGINT){
		
		struct timespec currentTime = { 0, 0 };

		FILE *pLogFile = fopen("logfile.txt", "a");
		pLogFile = fopen("logfile.txt", "a");
		if (pLogFile == NULL) {
			perror("ERROR: fopen");
			exit(1);
		}

		fprintf(pLogFile, "SIGINT signal Received...EXITING...");

		fclose(pLogFile);

		exit(1);
	}
	

}

int setupSigactionHandler(){

	struct sigaction act;

	memset(&act, 0, sizeof(act));

	act.sa_sigaction = signalHandler;
	act.sa_flags = SA_SIGINFO;

	if((sigaction(SIGINT, &act, NULL)) < 0){
			printf("ERROR - Registering USR1\n");
			return 1;
	}

	return 0;
}

/*
 * @brief: this function generates random string from the predefined list of characters
 * 			and given size of the string
 * @param randomString: pointer to store generated random string
 * @param stringLength: length of the string to be generated
 * @return void
 *
 * */

void randomStringGenerator(char *randomString, int stringLength){

	 char characters[] = "abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (stringLength-- > 0) {
        int index = (double) rand() / RAND_MAX * (sizeof characters - 1);
        *randomString++ = characters[index];
    }

    *randomString = '\0';

}

/*
 * @brief: this data sends data to the client
 * @param: fd: file descriptor for the socket
 * */
void sendDataToClient(int fd){

	int freq = 10;
		
	struct timespec currentTime = { 0, 0 };

	pLogFile = fopen("logfile.txt", "a");
	if (pLogFile == NULL) {
		perror("ERROR: fopen");
		exit(0);
	}
	clock_gettime(CLOCK_MONOTONIC, &currentTime);
	fprintf(pLogFile,
			"[%ld]IPC - Sockets\nServer Process ID: %d\nAllocated File Descriptor: %d\n",
			(currentTime.tv_sec*NSEC_PER_SEC) + (currentTime.tv_nsec),getpid(), fd);
	
	processData_t data;

	/*this loop sends data to the client*/
	for(int i = 0; i < freq; i++){

		/*generate a random string to be sent*/
		int size = rand() % 31;
		char dataString[size]; 
		randomStringGenerator(dataString, size);

		strcpy(data.string, dataString);
		data.ledStatus = LED_OFF;
		data.stringLength = strlen(dataString);

		clock_gettime(CLOCK_MONOTONIC, &currentTime);
		fprintf(pLogFile, "[%ld]Server Sending: String - %s stringLength - %d, LED status - %d\n", 
			(currentTime.tv_sec*NSEC_PER_SEC) + (currentTime.tv_nsec), data.string, data.stringLength, data.ledStatus);

		/*send data to the client*/
		if(-1 == (send(fd, (const void*)&data, sizeof(data), 0))){
			perror("ERROR: send");
			exit(1);
		}

		sleep(1);
	}

	fclose(pLogFile);
}

/*
 * @brief: this data reads data sent by the client
 * @param: fd: file descriptor for the socket
 * */
void readDataFromClient(int fd){

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
			"[%ld]IPC - Sockets\nServer Process ID: %d\nAllocated File Descriptor: %d\n",
			(currentTime.tv_sec*NSEC_PER_SEC) + (currentTime.tv_nsec),getpid(), fd);

	do{
		/*read data*/
		bytesReceived = read(fd, dataReceived, sizeof(dataReceived));

		processData_t *data = (processData_t*)(dataReceived);
		
		clock_gettime(CLOCK_MONOTONIC, &currentTime);

		fprintf(pLogFile, "[%ld]Server Receiving: String - %s stringLength - %d, LED status - %d\n", 
			(currentTime.tv_sec*NSEC_PER_SEC) + (currentTime.tv_nsec), data->string, data->stringLength, data->ledStatus);
		//printf("Data Received: String %s stringLength %d LED %d\n", data->string, data->stringLength, data->ledStatus);

		count--;

	}while(count);


	fclose(pLogFile);

}

int main(void){

	int socketFD = 0;
	int clientSocketFD = 0;
	int setReuseAddress = 1;

	struct sockaddr_in server;
	struct sockaddr_in client;

	socklen_t clientAddLen;


	//setupSigactionHandler();
	/*Create a new socket
	 * domain - IPv4, type - stream based socket, protocol - 0 a single type of
	 * protocol of family IPv4
	 * */

	if(0 > (socketFD = socket(AF_INET, SOCK_STREAM, 0))){
		perror("ERROR: socket");
		exit(1);
	}

	/*Set socket options*/
	if(-1 == (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &setReuseAddress, sizeof(setReuseAddress)))){
		perror("ERROR: setsocket");
		exit(1);
	}

	/*Configure server socket address*/
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(SERVER_PORT);

	/*Bind the socket*/
	if(0 > bind(socketFD, (struct sockaddr *)&server, sizeof(server))){
		perror("ERROR: bind");
		exit(1);
	}

	/*Listen on the socket, maximum pending connection is 1*/
	if(0 > listen(socketFD, 1)){
		perror("ERROR: listen");
		exit(1);
	}
	else{
		printf("listening on port %d\n", SERVER_PORT);
	}

	clientAddLen = sizeof(client);

	/*Accept a new client connection*/
	if(0 > (clientSocketFD = accept(socketFD, (struct sockaddr *)&client, &clientAddLen))){
		perror("ERROR: accept");
	}
	else{
		printf("Client Connected\n");
	}

	
	/*Send data to the client*/
	sendDataToClient(clientSocketFD);

	/*Read data from the client*/
	readDataFromClient(clientSocketFD);


	close(socketFD);

	return 0;

}


