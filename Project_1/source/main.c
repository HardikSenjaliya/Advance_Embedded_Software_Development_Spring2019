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

int main(int argc, char **argv) {

	log_message_t msg;

	logfile_attr_t logfile;

	pthread_t light_sensor, temperature_sensor, logger_task, socket_task;

	/*	if(argc < 3){
	 printf("Invalid Commnad Line Arguments/No Arguments. Please provide two arguments"
	 "required filename followed by filepath\n"
	 "Usage: fileName.txt /directory/filepath");
	 exit(1);
	 }

	 strcpy(logfile.file_name, argv[1]);
	 strcpy(logfile.file_path, argv[2]);*/

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
	if (pthread_create(&logger_task, NULL, run_logger, (void*) &logfile)) {

		msg.log_level = 0;
		strcpy(msg.message, "ERROR: spawning logger thread");
		strcpy(msg.thread_name, THREAD_NAME);
		clock_gettime(CLOCK_MONOTONIC, &(msg.time_stamp));

		send_message(qDes, msg);
	}

	/*3. Socket task*/
	if (pthread_create(&socket_task, NULL, run_socket, NULL)) {

		msg.log_level = 0;
		strcpy(msg.message, "ERROR: spawning socket thread");
		strcpy(msg.thread_name, THREAD_NAME);
		clock_gettime(CLOCK_MONOTONIC, &(msg.time_stamp));

		send_message(qDes, msg);
	}


	pthread_join(light_sensor, NULL);
	pthread_join(temperature_sensor, NULL);
	pthread_join(logger_task, NULL);
	pthread_join(socket_task, NULL);

	return 0;
}

