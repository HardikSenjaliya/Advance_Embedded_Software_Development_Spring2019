/*
 * main.c
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */


#include "../include/main.h"


#define Q_NAME				"/qMaintoLog"


/**
 * @brief main function
 * @return 0
 */

int main(void){

	log_message_t message;

	pthread_t light_sensor, temperature_sensor, logger;

	mqd_t qDes = create_posix_mq(Q_NAME);

		strcpy(message.message, "ERROR: spawning light sensor thread");
		message.thread_id = syscall(SYS_gettid);
		clock_gettime(CLOCK_MONOTONIC, &(message.time_stamp));

		send_message(qDes, message);

	/*Spwan new threads*/

	/*1. light sensor thread*/
	if(pthread_create(&light_sensor, NULL, run_light_sensor, NULL)){

		strcpy(message.message, "ERROR: spawning light sensor thread");
		message.thread_id = syscall(SYS_gettid);
		clock_gettime(CLOCK_MONOTONIC, &(message.time_stamp));

		send_message(qDes, message);
	}

	/*2. temperature sensor task*/
	if(pthread_create(&temperature_sensor, NULL, run_temperature_sensor, NULL))
		printf("ERROR: spawning temperature sensor thread\n");

	/*3. Logger task*/
	if(pthread_create(&logger, NULL, run_logger, NULL))
		printf("ERROR: spawning logger thread\n");


	pthread_join(light_sensor, NULL);
	pthread_join(temperature_sensor, NULL);
	pthread_join(logger, NULL);


	return 0;
}


