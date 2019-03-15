/*
 * light.c
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#include "../include/light.h"

#define Q_NAME				"/qLightToLog"
#define THREAD_NAME			"LIGHT_SENSOR"
#define PERIOD				(3)

/**
 * @brief this function is the thread function for the thread light_sensor
 * @param params
 * @return NULL
 */
void *run_light_sensor(void *params) {

	INFO_STDOUT("Light sensor thread started running...\n");
	mqd_t qDes = create_posix_mq(Q_NAME);

	log_message_t msg;
	msg.log_level = 0;
	strcpy(msg.message, "Hello From Light Sensor task");
	strcpy(msg.thread_name, THREAD_NAME);
	clock_gettime(CLOCK_MONOTONIC, &msg.time_stamp);
	send_message(qDes, msg);

	while (1) {
		sem_wait(&sem_light);
		send_message(qDes, msg);
	}

	return NULL;
}
