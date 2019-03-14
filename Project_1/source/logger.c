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
 * @brief this function creates a backup/copy of the source file
 * @param source_file
 * @return void
 */

void file_backup(char *source_file){

	char c;

	/*Open a new destination file*/
	FILE *dest_file = fopen("backup.txt", "w");
	if(dest_file == NULL){
		INFO_STDOUT("Error in opening backup file\n");
		exit(1);
	}

	/*Open the source file*/
	FILE *src_file = fopen(source_file, "r");
	if(src_file == NULL){
		INFO_STDOUT("Error in opening the source file\n");
		exit(1);
	}

	/*Start copying the file*/
	while ((c = fgetc(src_file)) != EOF && !feof(src_file)){
		fputc(c, dest_file);
	}

	if(0 != remove(source_file)){
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

	//logfile_attr_t logfile = (logfile_attr_t*)params;

	/*Check if logfile with the given name is already present or not.
	 * if present -> create a backfile and write to the new file
	 * if not present -> create a new file*/
	if(0 == access("logfile.txt", F_OK)){
		INFO_STDOUT("Given logfile name already present in the directory\n");
		INFO_STDOUT("Started Back up...");
		file_backup("logfile.txt");
		INFO_STDOUT("Done\n");
	}

	/*Create a new logfile*/
	FILE *p_logfile = fopen("logfile.txt", "a");
	if(p_logfile == NULL){
		INFO_STDOUT("Error in opening new logfile\n");
	}

	/*Open message queues for reading the messages from all tasks*/
	mqd_t qDesMain = create_posix_mq(Q_NAME_MAIN);
	mqd_t qDesLight = create_posix_mq(Q_NAME_LIGHT);
	mqd_t qDesTemp = create_posix_mq(Q_NAME_TEMP);

	int bytes_read = 0;
	log_message_t read_message;

	/*Start reading messages*/
	while (1) {

		/*Read messages from the Main task and wrtie to the logfile*/
		bytes_read = mq_receive(qDesMain, (char*) &read_message,
				sizeof(read_message), 0);

		if (bytes_read < 0) {
			perror("ERROR: mq_receive main task");
		} else {
			pthread_mutex_lock(&fileMutex);
			INFO_STDOUT("logging data from main task\n");
			log_message(p_logfile,
					(read_message.time_stamp.tv_sec) * NSEC_PER_SEC
							+ (read_message.time_stamp.tv_nsec),
					read_message.thread_name, read_message.log_level,
					read_message.message);

			pthread_mutex_unlock(&fileMutex);
		}

		/*Read messages from the light sensor task and wrtie to the logfile*/
		bytes_read = mq_receive(qDesLight, (char*) &read_message,
				sizeof(read_message), 0);

		if (bytes_read < 0) {
			perror("ERROR: mq_receive light task");
		} else {
			pthread_mutex_lock(&fileMutex);
			INFO_STDOUT("logging data from light task\n");
			log_message(p_logfile,
					(read_message.time_stamp.tv_sec) * NSEC_PER_SEC
							+ (read_message.time_stamp.tv_nsec),
					read_message.thread_name, read_message.log_level,
					read_message.message);
			pthread_mutex_unlock(&fileMutex);
		}

		/*Read messages from the temperature sensor task and wrtie to the logfile*/
		bytes_read = mq_receive(qDesTemp, (char*) &read_message,
				sizeof(read_message), 0);

		if (bytes_read < 0) {
			perror("ERROR: mq_receive temperature task");
		} else {
			pthread_mutex_lock(&fileMutex);
			INFO_STDOUT("logging data from temperature task\n");
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
