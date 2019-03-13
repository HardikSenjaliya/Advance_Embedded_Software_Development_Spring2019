/*
 * utils.c
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#include "../include/utils.h"

/**
 * @brief this function writes error log messages to the logfile
 * @param fp - pointer to the logfile
 * @param file - logfile name
 * @param line - line number in the source file
 * @param message - message to be logged in to the logfile
 * @return void
 */
void log_error(FILE *fp, char *file, int line, char *message){

	fprintf(fp, "%s:%d %s", file, line, message);
}





/**
 * @brief this function creates a new posix message queue OR opens already created
 *  		to sned and receive messages between threads
 * @param qName
 * @return 0 if successful, 1 otherwise
 */
mqd_t create_posix_mq(char *qName){

	mqd_t qDes;

	struct mq_attr attr;

	attr.mq_flags = 0;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = sizeof(log_message_t);

	/*Open a new posix queue*/
	qDes = mq_open(qName, O_CREAT | O_RDWR, 0666, &attr);

	if(qDes == -1){

		perror("ERROR: mq_open\n");
		exit(1);
	}

	return qDes;
}

/**
 * @brief this function sends messages to the message queue identified by the qDes
 * @param qDes file descriptior for the posix queue
 * @param message message to be logged into the logfile
 * @return 0 if successful, 1 otherwise
 */


int send_message(mqd_t qDes, log_message_t message){

	int bytes_sent = 0;

	bytes_sent = mq_send(qDes, (const char*)&message, sizeof(message), 0);

	if(bytes_sent < 0){
		perror("ERROR: mq_send\n");
		return 1;
	}

	return 0;
}















