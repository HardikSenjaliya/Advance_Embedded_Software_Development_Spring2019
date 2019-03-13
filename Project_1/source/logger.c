/*
 * logger.c
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#include "../include/logger.h"
#define NSEC_PER_SEC 			(1000000000)
#define Q_NAME_MAIN				"/qMaintoLog"
#define Q_NAME_LIGHT			"/qLightToLog"
#define Q_NAME_TEMP				"/qTemperatureToLog"

pthread_mutex_t fileMutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief this function logs informative messages from the logger thread into the logfile
 * @param fp pointer to the logfile
 * @param time_stamp time stamp value of the message read
 * @param thread_id  id of the thread from which message was sent
 * @param log_level  level of the log
 * @param message    message sent by the thread
 * @return void
 */
void log_message(FILE *fp, long int time_stamp, char *thread_name,
		int log_level, char *message) {

	fprintf(fp, "[%ld] %s %s\n", time_stamp, thread_name, message);
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
	mqd_t qDesLight = create_posix_mq(Q_NAME_LIGHT);
	mqd_t qDesTemp = create_posix_mq(Q_NAME_TEMP);

	int bytes_read = 0;
	log_message_t read_message;

	while (1) {

		bytes_read = mq_receive(qDesMain, (char*) &read_message,
				sizeof(read_message), 0);

		if (bytes_read < 0) {
			perror("ERROR: mq_receive");
		} else {
			pthread_mutex_lock(&fileMutex);

			log_message(p_logfile,
					(read_message.time_stamp.tv_sec) * NSEC_PER_SEC
							+ (read_message.time_stamp.tv_nsec),
					read_message.thread_name, read_message.log_level,
					read_message.message);

			pthread_mutex_unlock(&fileMutex);
		}

		bytes_read = mq_receive(qDesLight, (char*) &read_message,
				sizeof(read_message), 0);

		if (bytes_read < 0) {
			perror("ERROR: mq_receive");
		} else {
			pthread_mutex_lock(&fileMutex);
			log_message(p_logfile,
					(read_message.time_stamp.tv_sec) * NSEC_PER_SEC
							+ (read_message.time_stamp.tv_nsec),
					read_message.thread_name, read_message.log_level,
					read_message.message);
			pthread_mutex_unlock(&fileMutex);
		}

		bytes_read = mq_receive(qDesTemp, (char*) &read_message,
				sizeof(read_message), 0);

		if (bytes_read < 0) {
			perror("ERROR: mq_receive");
		} else {
			pthread_mutex_lock(&fileMutex);
			log_message(p_logfile,
					(read_message.time_stamp.tv_sec) * NSEC_PER_SEC
							+ (read_message.time_stamp.tv_nsec),
					read_message.thread_name, read_message.log_level,
					read_message.message);
			pthread_mutex_unlock(&fileMutex);
		}
	}

	fclose(p_logfile);

	return NULL;
}
