/*
 * main.h
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#ifndef SOURCE_MAIN_H_
#define SOURCE_MAIN_H_

#include "../include/utils.h"
#include "../include/light.h"
#include "../include/temperature.h"
#include "../include/logger.h"
#include "../include/socket.h"

#define HEARTBEAT_REQUESTS				(3)

typedef struct{

	mqd_t qDesMain;
	mqd_t qDesLight;
	mqd_t qDesTemp;
	mqd_t qDesLogger;
	mqd_t qDesSocket;
}queue_descriptors_t;

int check_thread_status(mqd_t qDes);
int read_thread_status(mqd_t qDesMain);
void send_exit_command();
#endif /* SOURCE_MAIN_H_ */
