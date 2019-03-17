/*
 * utils.h
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#ifndef SOURCE_UTILS_H_
#define SOURCE_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <mqueue.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/syscall.h>

#define Q_NAME_MAIN							"/qMain"
#define Q_NAME_LIGHT						"/qLight"
#define Q_NAME_TEMP							"/qTemperature"
#define Q_NAME_LOGGER						"/qLogger"
#define Q_NAME_SOCKET						"/qSocket"
#define MAIN_THREAD_NAME					"MAIN_THREAD"
#define LIGHT_THREAD_NAME					"LIGHT_THREAD"
#define TEMP_THREAD_NAME					"TEMP_THREAD"
#define LOGGER_THREAD_NAME					"LOGGER_THREAD"
#define SOCKET_THREAD_NAME					"SOCKET_THREAD"
#define LIGHT_THREAD_ALIVE					(1111)
#define TEMP_THREAD_ALIVE					(2222)
#define LOGGER_THREAD_ALIVE					(3333)
#define SOCKET_THREAD_ALIVE					(4444)
#define MSEC_TO_NSEC						(1000000)
#define NSEC_TO_SEC							(1000000000)


/*Macro functions to print information and error messages on the console*/
#define ERROR_STDOUT(error) fprintf(stdout, "%s:%d %s", __FILE__, __LINE__, error)
#define INFO_STDOUT(message) fprintf(stdout, "%s:%d %s", __FILE__, __LINE__, message)

sem_t sem_light, sem_temp, sem_heartbeat;

typedef enum{

	SEND_ALIVE_STATUS = 11,
	TIME_TO_EXIT = 21,

}request_from_main;


/*Structure to log the information in the logfile*/
typedef struct {

	int log_level;
	struct timespec time_stamp;
	char thread_name[20];
	char message[100];
	request_from_main req_type;
	int alive_status;

} log_message_t;

/*function prototypes*/

mqd_t create_posix_mq(char *qName);
int send_message(mqd_t qDes, log_message_t *message);
int initialize_semaphores(void);
int start_timer(void);

#endif /* SOURCE_UTILS_H_ */
