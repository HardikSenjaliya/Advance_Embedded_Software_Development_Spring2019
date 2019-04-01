/*
 * logger.c
 *  @brief this file is the thread function for logger task. It accepts all
 *  the messages from tasks and logs them into the logfile.
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#include "../include/logger.h"

#define DEBUG_MODE

extern int startup_request;
extern pthread_mutex_t startup_mutex;

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

	logfile_attr_t *logfile = (logfile_attr_t*) params;

	/*Check if logfile with the given name is already present or not.
	 * if present -> create a backfile and write to the new file
	 * if not present -> create a new file*/
	if (0 == access(logfile->file_name, F_OK)) {
		//INFO_STDOUT("Given logfile name already present in the directory\n");
		//INFO_STDOUT("Started Back up...\n");
		file_backup(logfile->file_name);
		//INFO_STDOUT("Backup Done\n");
	}

	/*Create a new logfile*/
	FILE *p_logfile = fopen(logfile->file_name, "a");
	if (p_logfile == NULL) {
		INFO_STDOUT("Error in opening new logfile\n");
	}

	/*Create a POSIX message queue to send requests to child threads*/
	mqd_t qDesLogger = create_logger_mq();
	mqd_t qDesMain = create_main_mq();

	int received_bytes = 0, send_status = 0;
	log_message_t request;
	heartbeat_response_t response;

	/*Start reading messages*/
	while (1) {

		/*Read messages from the Main task and wrtie to the logfile*/
		received_bytes = mq_receive(qDesLogger, (char*) &request,
				sizeof(request), 0);

		if (received_bytes < 0) {
			//perror("LOGGER THREAD");
		} else {
			switch (request.req_type) {
			case SEND_ALIVE_STATUS:
				strcpy(response.thread_name, LOGGER_THREAD_NAME);
				response.alive_status = LOGGER_THREAD_ALIVE;

				send_status = mq_send(qDesMain, (const char*) &response,
						sizeof(response), P_WARNING);
				if (send_status < 0) {
					//perror("LOGGER THREAD");
				} else {
					//INFO_STDOUT("LOGGER-response to hearbeat request sent\n");
					request.req_type = 0;
				}
				break;

			case STARTUP_TEST:

				pthread_mutex_lock(&startup_mutex);
				startup_request |= (1 << 0);
				pthread_mutex_unlock(&startup_mutex);

				strcpy(response.thread_name, LOGGER_THREAD_NAME);
				response.alive_status = LOGGER_THREAD_ALIVE;

				send_status = mq_send(qDesMain, (const char*) &response,
						sizeof(response), P_WARNING);
				if (send_status < 0) {
					//perror("LOGGER THREAD");
				} else {
					//INFO_STDOUT("LOGGER-response to hearbeat request sent\n");
					request.req_type = 0;
				}

				break;

			case TIME_TO_EXIT:
				//INFO_STDOUT("LOGGER_THREAD: Request to Exit received\n");
				fprintf(p_logfile,
						"[%ld] *** From : %s *** Log Level : %d *** Received Message : %s\n",
						(request.time_stamp.tv_sec) * NSEC_TO_SEC
								+ (request.time_stamp.tv_nsec),
						request.thread_name, request.log_level,
						request.message);

				goto EXIT;
				break;
			default:
				break;

			}

			pthread_mutex_lock(&fileMutex);
			//INFO_STDOUT("logging data into logfile\n");

			fprintf(p_logfile,
					"[%ld] *** From : %s *** Log Level : %d *** Received Message : %s\n",
					(request.time_stamp.tv_sec) * NSEC_TO_SEC
							+ (request.time_stamp.tv_nsec), request.thread_name,
					request.log_level, request.message);

			/*
			 printf(
			 "[%ld] *** From : %s *** Log Level : %d *** Received Message : %s\n",
			 (request.time_stamp.tv_sec) * NSEC_TO_SEC
			 + (request.time_stamp.tv_nsec), request.thread_name,
			 request.log_level, request.message);
			 */

			/*

			 if (request.log_level != L_WARN) {
			 fprintf(p_logfile,
			 "[%ld] *** From : %s *** Log Level : %d *** Received Message : %s\n",
			 (request.time_stamp.tv_sec) * NSEC_TO_SEC
			 + (request.time_stamp.tv_nsec),
			 request.thread_name, request.log_level,
			 request.message);
			 }

			 #ifdef INFO_MODE
			 if (request.log_level == L_INFO) {
			 printf("%s\n", request.message);
			 }
			 #endif

			 if (request.log_level == L_ERROR) {
			 INFO_STDOUT(request.message);
			 }
			 */

			pthread_mutex_unlock(&fileMutex);
		}
	}

	EXIT:

	fclose(p_logfile);
	mq_unlink(Q_NAME_LOGGER);
	pthread_mutex_destroy(&fileMutex);
	INFO_STDOUT("LOGGER_THREAD...EXITING\n");
	return NULL;
}
