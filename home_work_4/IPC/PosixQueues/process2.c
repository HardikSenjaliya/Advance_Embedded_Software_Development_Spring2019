/*
 * pQueues.c
 *
 *  Created on: Feb 27, 2019
 *      Author: hardyk
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>

#define Q_NAME				"/myMessageQ"
#define Q_SIZE				(10)

void sendMessage(mqd_t);

typedef enum{
	LED_OFF,
	LED_ON
}ledStatus;

typedef struct{

	char string[20];
	int stringLength;
	int ledStatus;
}processData_t;

void sendMessage(mqd_t myQdes){

	int count = 5;

	char *message = "Hello from process 2";

	processData_t sendingData;
	strcpy(sendingData.string, message);
	sendingData.stringLength = strlen(message);
	sendingData.ledStatus = LED_OFF;

	
	do{

		if(-1 == mq_send(myQdes, (const char*)&sendingData, sizeof(sendingData), 0)){
			perror("ERROR: mq_send");
			exit(1);
		}

		count--;
		
	}while(count);


}

void receiveMessage(mqd_t myQdes){


	int count = 5;

	processData_t receivedData;

	do{
	if(-1 == mq_receive(myQdes, (char*)&receivedData, sizeof(receivedData), 0)){
		perror("ERROR: mq_receive");
		exit(1);
	}

	printf("Received Data: String = %s, String Length = %d, LED Status = %d\n", receivedData.string,
			receivedData.stringLength, receivedData.ledStatus);

	count--;
	}while(count);

}

int main(void){

	struct mq_attr myQattr;
	myQattr.mq_maxmsg = Q_SIZE;
	myQattr.mq_msgsize = sizeof(processData_t);

	/*Open a queue for reading and writing*/
	mqd_t myQdes = mq_open(Q_NAME, O_CREAT | O_RDWR, 0666, &myQattr);

	if(myQdes == (mqd_t) - 1){
		perror("ERROR: mq_open");
		exit(1);
	}

	receiveMessage(myQdes);
	sendMessage(myQdes);

	if((mq_close(myQdes)) == -1)
		perror("ERROR: mq_close");
	if((mq_unlink(Q_NAME)) == -1)
		perror("ERROR: mq_unlink");

	return 0;

}
