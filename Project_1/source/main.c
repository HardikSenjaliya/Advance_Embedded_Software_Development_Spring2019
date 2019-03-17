/*
 * main.c
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#include "../include/main.h"

int heartbeat_request = 0;

bool light_status = 0, temp_status = 0, logger_status = 0, socket_status = 0;

/**
 * @brief this function sends exit command to all the threads after one/more thread found
 * to be not running as expected
 * @param p_descriptors pointer to the structure which contains all the descriptors of
 * POSIX message queues
 * @return void
 */
void send_exit_command(queue_descriptors_t *p_descriptors) {

	log_message_t request;
	int send_status = 0;
	request.req_type = TIME_TO_EXIT;

	send_status = mq_send(p_descriptors->qDesLight, (const char*) &request,
			sizeof(request), 1);
	if (send_status < 0) {
		//ERROR_STDOUT("ERROR: while sending request for the hearbeat\n");
	}

	send_status = mq_send(p_descriptors->qDesTemp, (const char*) &request,
			sizeof(request), 1);
	if (send_status < 0) {
		//ERROR_STDOUT("ERROR: while sending request for the hearbeat\n");
	}

	send_status = mq_send(p_descriptors->qDesLogger, (const char*) &request,
			sizeof(request), 1);
	if (send_status < 0) {
		//ERROR_STDOUT("ERROR: while sending request for the hearbeat\n");
	}

	send_status = mq_send(p_descriptors->qDesSocket, (const char*) &request,
			sizeof(request), 1);
	if (send_status < 0) {
		//ERROR_STDOUT("ERROR: while sending request for the hearbeat\n");
	}
}

/**
 * @brief this function reads messages the POSIX message queue of the given child thread.
 * @param qDesMain file descriptor of the message queue from which message is to be read
 * @return 0 if successful
 */
int read_thread_status(mqd_t qDesMain) {

	int bytes_recevied = 0;

	log_message_t response;

	bytes_recevied = mq_receive(qDesMain, (char*) &response, sizeof(response),
			0);
	if (bytes_recevied < 0) {
		//ERROR_STDOUT("ERROR: while reading responses from the MAIN Q\n");
	}
	printf("Response Read from the MAIN Q: sent by Q - %s\n",
			response.thread_name);

	if (response.alive_status == LIGHT_THREAD_ALIVE) {
		light_status = 1;
	}

	if (response.alive_status == TEMP_THREAD_ALIVE) {
		temp_status = 1;
	}

	if (response.alive_status == LOGGER_THREAD_ALIVE) {
		logger_status = 1;
	}

	if (response.alive_status == SOCKET_THREAD_ALIVE) {
		socket_status = 1;
	}

	return 0;
}

/**
 * @brief this function sends request to a given thread to check if the thread is alive or not
 * @param qDes file descriptor of the message queue on which message is to be sent
 * @return 0 if successful
 */
int check_thread_status(mqd_t qDes) {

	int send_status = 0;

	log_message_t request;

	request.req_type = SEND_ALIVE_STATUS;

	send_status = mq_send(qDes, (const char*) &request, sizeof(request), 1);
	if (send_status < 0) {
		//ERROR_STDOUT("ERROR: while sending request for the hearbeat\n");
	}

	/*this sleep is not required, here for testing purposes*/
	sleep(1);
	return 0;
}

/**
 * @brief main function
 * @return 0
 */

int main(int argc, char **argv) {

	int heartbeat = 1;

	log_message_t msg;

	logfile_attr_t logfile;
	queue_descriptors_t queue_descriptors;

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

	mqd_t qDesMain = create_posix_mq(Q_NAME_MAIN);
	mqd_t qDesLogger = create_posix_mq(Q_NAME_LOGGER);
	mqd_t qDesLight = create_posix_mq(Q_NAME_LIGHT);
	mqd_t qDesTemp = create_posix_mq(Q_NAME_TEMP);
	mqd_t qDesSocket = create_posix_mq(Q_NAME_SOCKET);

	queue_descriptors.qDesMain = qDesMain;
	queue_descriptors.qDesLight = qDesLight;
	queue_descriptors.qDesTemp = qDesTemp;
	queue_descriptors.qDesLogger = qDesLogger;
	queue_descriptors.qDesSocket = qDesSocket;

	msg.log_level = 0;
	strcpy(msg.message, "Hello from main task");
	strcpy(msg.thread_name, MAIN_THREAD_NAME);
	clock_gettime(CLOCK_MONOTONIC, &msg.time_stamp);

	send_message(qDesLogger, &msg);

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

	while (heartbeat) {
		sem_wait(&sem_heartbeat);
		heartbeat_request = 1;
		check_thread_status(qDesLight);
		read_thread_status(qDesMain);
		check_thread_status(qDesTemp);
		read_thread_status(qDesMain);
		check_thread_status(qDesLogger);
		read_thread_status(qDesMain);
		check_thread_status(qDesSocket);
		read_thread_status(qDesMain);

		if (light_status < 1 || temp_status < 1 || logger_status < 1
				|| socket_status < 1) {
			INFO_STDOUT("One or more thread died\n");
			heartbeat = 0;
			send_exit_command(&queue_descriptors);
		} else {
			INFO_STDOUT("All threads are running\n");
		}
	}

	pthread_join(light_sensor, NULL);
	pthread_join(temperature_sensor, NULL);
	pthread_join(logger_task, NULL);
	pthread_join(socket_task, NULL);

	INFO_STDOUT("Main thread Exiting...Bye Bye...\n");

	return 0;
}

