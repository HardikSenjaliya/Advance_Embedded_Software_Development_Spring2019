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

	if (argc < 2) {
		printf(
				"Invalid Commnad Line Arguments/No Arguments. Please provide required arguments"
						"Usage: filepath/filename.txt");
		exit(1);
	}

	strcpy(logfile.file_name, argv[1]);

	if (initialize_semaphores()) {
		ERROR_STDOUT("ERROR: initializing semaphores\n");
		exit(1);
	}

	mqd_t qDes = create_posix_mq(Q_NAME);

	msg.log_level = 0;
	strcpy(msg.message, "Hello from main task");
	strcpy(msg.thread_name, THREAD_NAME);
	clock_gettime(CLOCK_MONOTONIC, &msg.time_stamp);

	send_message(qDes, msg);

	/*Spwan new threads*/
	INFO_STDOUT("Creating new threads\n");

	/*1. light sensor thread*/
	if (pthread_create(&light_sensor, NULL, run_light_sensor, NULL)) {
		ERROR_STDOUT("ERROR: creating light sensor thread\n");
	}

	/*2. temperature sensor task*/
	if (pthread_create(&temperature_sensor, NULL, run_temperature_sensor,
	NULL)) {
		ERROR_STDOUT("ERROR: creating temperature sensor thread\n");
	}

	/*3. Logger task*/
	if (pthread_create(&logger_task, NULL, run_logger, (void*) &logfile)) {
		ERROR_STDOUT("ERROR: creating logger task thread\n");
	}

	/*3. Socket task*/
	if (pthread_create(&socket_task, NULL, run_socket, NULL)) {
		ERROR_STDOUT("ERROR: creating socket task thread\n");
	}

	if (start_timer()) {
		ERROR_STDOUT("ERROR: starting timer");
		exit(1);
	}

	pthread_join(light_sensor, NULL);
	pthread_join(temperature_sensor, NULL);
	pthread_join(logger_task, NULL);
	pthread_join(socket_task, NULL);

	return 0;
}

