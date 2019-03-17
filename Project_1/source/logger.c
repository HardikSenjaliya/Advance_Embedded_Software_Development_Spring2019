/*
 * logger.c
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#include "../include/logger.h"


pthread_mutex_t fileMutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief this function creates a backup/copy of the source file
 * @param source_file
 * @return void
 */

void file_backup(char *source_file) {

	char c;

	/*Open a new destination file*/
	FILE *dest_file = fopen("backup.txt", "w");
	if (dest_file == NULL) {
		INFO_STDOUT("Error in opening backup file\n");
		exit(1);
	}

	/*Open the source file*/
	FILE *src_file = fopen(source_file, "r");
	if (src_file == NULL) {
		INFO_STDOUT("Error in opening the source file\n");
		exit(1);
	}

	/*Start copying the file*/
	while ((c = fgetc(src_file)) != EOF && !feof(src_file)) {
		fputc(c, dest_file);
	}

	if (0 != remove(source_file)) {
		INFO_STDOUT("Error in deleting old file\n");
	}

}

/**
 * @brief this function logs informative messages from the logger thread into the logfile
 * @param fp pointer to the logfile
 * @param time_stamp time stamp value of the message read
 * @param thread_name  name of the thread from which message was sent
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

	INFO_STDOUT("Logger thread started running...\n");

	//logfile_attr_t logfile = (logfile_attr_t*)params;

	/*Check if logfile with the given name is already present or not.
	 * if present -> create a backfile and write to the new file
	 * if not present -> create a new file*/
	if (0 == access("logfile.txt", F_OK)) {
		INFO_STDOUT("Given logfile name already present in the directory\n");
		INFO_STDOUT("Started Back up...");
		file_backup("logfile.txt");
		INFO_STDOUT("Backup Done\n");
	}

	/*Create a new logfile*/
	FILE *p_logfile = fopen("logfile.txt", "a");
	if (p_logfile == NULL) {
		INFO_STDOUT("Error in opening new logfile\n");
	}

	/*Open message queues for reading the messages from all tasks*/
	mqd_t qDesLogger = create_posix_mq(Q_NAME_LOGGER);
	mqd_t qDesMain = create_posix_mq(Q_NAME_MAIN);

	int received_bytes = 0, send_status = 0;
	log_message_t request, response;

	/*Start reading messages*/
	while (1) {

		/*Read messages from the Main task and wrtie to the logfile*/
		received_bytes = mq_receive(qDesLogger, (char*) &request,
				sizeof(request), 0);

		if (received_bytes < 0) {
			//perror("ERROR: mq_receive main task");
		} else {

			received_bytes = mq_receive(qDesLogger, (char*) &request,
					sizeof(request), 0);
			if (received_bytes < 0) {
				//ERROR_STDOUT("ERROR: while reading responses from the LIGHT Q\n");
			}

			if (request.req_type == SEND_ALIVE_STATUS) {
				strcpy(response.thread_name, LOGGER_THREAD_NAME);
				response.alive_status = LOGGER_THREAD_ALIVE;

				send_status = mq_send(qDesMain, (const char*) &response,
						sizeof(response), 1);
				if (send_status < 0) {
					//ERROR_STDOUT("ERROR: while sending responses to the MAIN Q from LIGHT THREAD\n");
				}

				request.req_type = 0;
			}

			if(request.req_type == TIME_TO_EXIT){
				INFO_STDOUT("Request to Exit received\n");
				goto EXIT;
			}

			pthread_mutex_lock(&fileMutex);
			//INFO_STDOUT("logging data from main task\n");
			log_message(p_logfile,
					(request.time_stamp.tv_sec) * NSEC_TO_SEC
							+ (request.time_stamp.tv_nsec), request.thread_name,
					request.log_level, request.message);

			pthread_mutex_unlock(&fileMutex);
		}
	}

	fclose(p_logfile);

	EXIT:
	fclose(p_logfile);
	pthread_mutex_destroy(&fileMutex);
	return NULL;
}
