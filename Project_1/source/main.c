/*
 * main.c
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#include "../include/main.h"

#define Q_NAME				"/qMaintoLog"
#define THREAD_NAME			"MAIN"

/**
 * @brief main function
 * @return 0
 */

int main(void) {

	log_message_t msg;

	pthread_t light_sensor, temperature_sensor, logger;

	mqd_t qDes = create_posix_mq(Q_NAME);

	/*Spwan new threads*/

	/*1. light sensor thread*/
	if (pthread_create(&light_sensor, NULL, run_light_sensor, NULL)) {

		msg.log_level = 0;
		strcpy(msg.message, "ERROR: spawning light sensor thread");
		strcpy(msg.thread_name, THREAD_NAME);
		clock_gettime(CLOCK_MONOTONIC, &(msg.time_stamp));

		send_message(qDes, msg);
	}

	/*2. temperature sensor task*/
	if (pthread_create(&temperature_sensor, NULL, run_temperature_sensor,
	NULL)) {

		msg.log_level = 0;
		strcpy(msg.message, "ERROR: spawning temp sensor thread");
		strcpy(msg.thread_name, THREAD_NAME);
		clock_gettime(CLOCK_MONOTONIC, &(msg.time_stamp));

		send_message(qDes, msg);
	}

	/*3. Logger task*/
	if (pthread_create(&logger, NULL, run_logger, NULL)) {

		msg.log_level = 0;
		strcpy(msg.message, "ERROR: spawning logger thread");
		strcpy(msg.thread_name, THREAD_NAME);
		clock_gettime(CLOCK_MONOTONIC, &(msg.time_stamp));

		send_message(qDes, msg);
	}

	pthread_join(light_sensor, NULL);
	pthread_join(temperature_sensor, NULL);
	pthread_join(logger, NULL);

	return 0;
}

