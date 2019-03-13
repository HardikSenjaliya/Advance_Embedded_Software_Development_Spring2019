/*
 * temperature.c
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#include "../include/temperature.h"

#define Q_NAME				"/qTemperatureToLog"
#define THREAD_NAME			"TEMP_SENSOR"

/**
 * @brief this function is the thread function for the thread temperature_sensor
 * @param params
 * @return NULL
 */
void *run_temperature_sensor(void *params) {

	mqd_t qDes = create_posix_mq(Q_NAME);

	log_message_t msg;
	msg.log_level = 0;
	strcpy(msg.message, "Hello From Temp Sensor task");
	strcpy(msg.thread_name, THREAD_NAME);
	clock_gettime(CLOCK_MONOTONIC, &msg.time_stamp);

	while (1) {
		send_message(qDes, msg);
	}

	return NULL;

}
