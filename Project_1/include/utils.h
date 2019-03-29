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
#define MESSAGE_SIZE						(60)
#define THREAD_NAME_SIZE					(13)


/*Macro functions to print information and error messages on the console*/
#define ERROR_STDOUT(error) fprintf(stdout, "%s:%d %s", __FILE__, __LINE__, error)
#define INFO_STDOUT(message) fprintf(stdout, "%s:%d %s", __FILE__, __LINE__, message)

sem_t sem_light, sem_temp, sem_heartbeat;

typedef enum{

	GET_TEMP_C = 1,
	GET_TEMP_F,
	GET_TEMP_K,
	GET_LUX,
	GET_LIGHT_STATUS,
	CLOSE_CONNECTION,
	SEND_ALIVE_STATUS,
	TIME_TO_EXIT

}request_type_t;

typedef enum{

	L_INFO = 0,
	L_WARN,
	L_ERROR,
	L_CRTICAL

}log_level_t;


/*Structure to log the information in the logfile*/
typedef struct {

	log_level_t log_level;
	struct timespec time_stamp;
	char thread_name[THREAD_NAME_SIZE];
	char message[MESSAGE_SIZE];
	request_type_t req_type;

} log_message_t;

typedef struct{

	char thread_name[THREAD_NAME_SIZE];
	int alive_status;

}heartbeat_response_t;

typedef struct{

	mqd_t qDesMain;
	mqd_t qDesLight;
	mqd_t qDesTemp;
	mqd_t qDesLogger;

}queue_descriptors_t;


typedef struct{

	char message[30];
	struct timespec time_stamp;
	request_type_t req_type;

}request_t;

typedef enum{

	P_INFO = 0,
	P_WARNING,
	P_ERROR,
	P_CRITICAL

}message_priority;


typedef enum{
	DAY = 0,
	NIGHT
}day_or_night_t;


typedef struct {

	char message[30];
	double data;

} client_request_response_t;


/*function prototypes*/

mqd_t create_light_mq(void);
mqd_t create_temp_mq(void);
mqd_t create_logger_mq(void);
mqd_t create_main_mq(void);
mqd_t create_socket_mq(void);
int initialize_semaphores(void);
int destroy_semaphores(void);
int start_timer(void);
int stop_timer(void);

#endif /* SOURCE_UTILS_H_ */
