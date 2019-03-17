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

int check_thread_heartbeats(mqd_t qDes);

#endif /* SOURCE_MAIN_H_ */
