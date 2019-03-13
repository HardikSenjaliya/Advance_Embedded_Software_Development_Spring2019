/*
 * logger.c
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#include "../include/logger.h"
#define NSEC_PER_SEC 			(1000000000)
#define Q_NAME_MAIN				"/qMaintoLog"

/**
 * @brief this function logs informative messages from the logger thread into the logfile
 * @param fp pointer to the logfile
 * @param time_stamp time stamp value of the message read
 * @param thread_id  id of the thread from which message was sent
 * @param log_level  level of the log
 * @param message    message sent by the thread
 * @return void
 */
void log_message(FILE *fp, long int time_stamp, unsigned int thread_id, int log_level, char *message){

	fprintf(fp, "[%ld] %u %s\n", time_stamp, thread_id, message);

	fclose(fp);
}



/**
 * @brief this function is the thread function for the thread logger
 * 			it enters all the logs recevied from other threads into the logfile
 * @param params thread function parameters
 * @return NULL
 */
void *run_logger(void *params) {

	FILE *p_logfile = fopen("logfile.txt", "a");

	mqd_t qDesMain = create_posix_mq(Q_NAME_MAIN);

	int bytes_read = 0;
	log_message_t read_message;

	while (1) {

		bytes_read = mq_receive(qDesMain, (char*) &read_message,
				sizeof(read_message), 0);

		if (bytes_read < 0) {
			perror("ERROR: mq_receive");
		}

		log_message(p_logfile,
				(read_message.time_stamp.tv_sec) * NSEC_PER_SEC
						+ (read_message.time_stamp.tv_nsec),
				read_message.thread_id, read_message.log_level,
				read_message.message);
	}

	return NULL;
}
