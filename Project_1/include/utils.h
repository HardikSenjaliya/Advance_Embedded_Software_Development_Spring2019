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
#include <sys/syscall.h>

#define ERROR_STDOUT(error) fprintf(stdout, "%s:%d %s", __FILE__, __LINE__, error)
#define INFO_STDOUT(message) fprintf(stdout, "%s:%d %s", __FILE__, __LINE__, message)

typedef struct{

	int log_level;
	struct timespec time_stamp;
	char thread_name[20];
	char message[100];

}log_message_t;


/*function prototypes*/

void _log(FILE *fp, char *file, int line, char *message);
mqd_t create_posix_mq(char *qName);
int send_message(mqd_t qDes, log_message_t message);
int receive_message();


#endif /* SOURCE_UTILS_H_ */
