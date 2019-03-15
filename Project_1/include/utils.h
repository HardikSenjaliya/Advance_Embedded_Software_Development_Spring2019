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
#include <string.h>
#include <mqueue.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/syscall.h>

#define MSEC_TO_NSEC						(1000000)
#define NSEC_TO_SEC							(1000000000)

/*Macro functions to print information and error messages on the console*/
#define ERROR_STDOUT(error) fprintf(stdout, "%s:%d %s", __FILE__, __LINE__, error)
#define INFO_STDOUT(message) fprintf(stdout, "%s:%d %s", __FILE__, __LINE__, message)

sem_t sem_light, sem_temp;

/*Structure to log the information in the logfile*/
typedef struct {

	int log_level;
	struct timespec time_stamp;
	char thread_name[20];
	char message[100];

} log_message_t;

/*function prototypes*/

mqd_t create_posix_mq(char *qName);
int send_message(mqd_t qDes, log_message_t message);
int initialize_semaphores(void);
int start_timer(void);

#endif /* SOURCE_UTILS_H_ */
