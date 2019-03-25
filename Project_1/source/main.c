/*
 * main.c
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#include "../include/main.h"

#define LETS_EXIT

int heartbeat_request = 0;

bool light_status = 0, temp_status = 0, logger_status = 0, socket_status = 0;

/**
 * @brief main function
 * @return 0
 */

int main(int argc, char **argv) {

	int heartbeat = 1;

	pthread_t light_sensor, temperature_sensor, logger_task, socket_task;

	log_message_t msg;
	request_type_t request;
	heartbeat_response_t response;

	logfile_attr_t logfile;

	int send_status = 0, received_bytes = 0;

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

	/*Create a POSIX message queue to send requests to child threads*/
	mqd_t qDesMain = create_main_mq();
	mqd_t qDesLogger = create_logger_mq();
	mqd_t qDesLight = create_light_mq();
	mqd_t qDesTemp = create_temp_mq();

	/*Spwan new threads*/
	INFO_STDOUT("Creating new threads\n");

	/*1. light sensor thread*/
	if (pthread_create(&light_sensor, NULL, run_light_sensor, NULL)) {
		ERROR_STDOUT("ERROR: creating light sensor thread\n");
		return 1;
	}

	/*2. temperature sensor task*/
	if (pthread_create(&temperature_sensor, NULL, run_temperature_sensor,
	NULL)) {
		ERROR_STDOUT("ERROR: creating temperature sensor thread\n");
		return 1;
	}

	/*3. Logger task*/
	if (pthread_create(&logger_task, NULL, run_logger, (void*) &logfile)) {
		ERROR_STDOUT("ERROR: creating logger task thread\n");
		return 1;
	}

	/*3. Socket task*/
	if (pthread_create(&socket_task, NULL, run_socket, NULL)) {
		ERROR_STDOUT("ERROR: creating socket task thread\n");
		return 1;
	}

	if (start_timer()) {
		ERROR_STDOUT("ERROR: starting timer");
		exit(1);
	}

	msg.log_level = 0;
	strcpy(msg.thread_name, MAIN_THREAD_NAME);
	clock_gettime(CLOCK_MONOTONIC, &msg.time_stamp);
	strcpy(msg.message, "Hello from main task");

	send_status = mq_send(qDesLogger, (const char*) &msg, sizeof(msg), 0);
	if (send_status < 0) {
		perror("MAIN THREAD");
	} else {
		INFO_STDOUT("MAIN_THREAD: message to logger sent\n");
	}

	/**TODO use mq_timedreceive() for reading heartbeat responses from all threads
	 * which will not block the exection if a thread is died and not able to send the response*/

	while (heartbeat) {

		sem_wait(&sem_heartbeat);

		/*Request status from light thread*/
		request.req_type = SEND_ALIVE_STATUS;
		clock_gettime(CLOCK_MONOTONIC, &request.time_stamp);

		send_status = mq_send(qDesLight, (const char*) &request,
				sizeof(request), 1);
		if (send_status < 0) {
			perror("MAIN_THREAD: heartbeat request");
		} else {
			//INFO_STDOUT("Heartbeat request to Light thread sent\n");
		}

		/*Read status sent by the light thread*/
		received_bytes = mq_receive(qDesMain, (char*) &response,
				sizeof(response), 0);
		if (received_bytes < 0) {
			//perror("MAIN THREAD: reading heartbeat response");
		} else {
			//printf("Response Received -> %d\n", response.alive_status);
			if (response.alive_status == LIGHT_THREAD_ALIVE) {
				light_status = 1;
			}
		}

		/*Request status from temp thread*/
		request.req_type = SEND_ALIVE_STATUS;
		clock_gettime(CLOCK_MONOTONIC, &request.time_stamp);

		send_status = mq_send(qDesTemp, (const char*) &request, sizeof(request),
				1);
		if (send_status < 0) {
			perror("MAIN_THREAD: heartbeat request");
		} else {
			//INFO_STDOUT("Heartbeat request to Temp thread sent\n");
		}

		/*Read status sent by the temp thread*/
		received_bytes = mq_receive(qDesMain, (char*) &response,
				sizeof(response), 0);
		if (received_bytes < 0) {
			//perror("MAIN THREAD: reading heartbeat response");
		} else {
			//printf("Response Received -> %d\n", response.alive_status);
			if (response.alive_status == TEMP_THREAD_ALIVE) {
				temp_status = 1;
			}
		}

		/*Request status from Logger thread*/
		log_message_t message;
		message.req_type = SEND_ALIVE_STATUS;
		clock_gettime(CLOCK_MONOTONIC, &message.time_stamp);

		send_status = mq_send(qDesLogger, (const char*) &message,
				sizeof(message), 1);
		if (send_status < 0) {
			perror("MAIN_THREAD: heartbeat request");
		} else {
			//INFO_STDOUT("Heartbeat request to Logger thread sent\n");
		}

		/*Read status sent by the temp thread*/
		received_bytes = mq_receive(qDesMain, (char*) &response,
				sizeof(response), 0);
		if (received_bytes < 0) {
			//perror("MAIN THREAD: reading heartbeat response");
		} else {
			//printf("Response Received -> %d\n", response.alive_status);
			if (response.alive_status == LOGGER_THREAD_ALIVE) {
				logger_status = 1;
			}
		}

#ifdef LETS_EXIT
		heartbeat = 0;
#endif

		/*Checking if response is not received from any thread and if not received then
		 * send exit commnad to all the threads*/
		if (light_status < 1 || temp_status < 1 || logger_status < 1) {
			ERROR_STDOUT("One or More thread died...SENDING EXIT COMMNAD\n");

			/*Request status from light thread*/
			request.req_type = TIME_TO_EXIT;
			clock_gettime(CLOCK_MONOTONIC, &request.time_stamp);

			send_status = mq_send(qDesLight, (const char*) &request,
					sizeof(request), 1);
			if (send_status < 0) {
				//perror("MAIN_THREAD: exit request");
			} else {
				INFO_STDOUT("EXIT request to Light thread sent\n");
			}

			/*Request status from temp thread*/
			request.req_type = TIME_TO_EXIT;
			clock_gettime(CLOCK_MONOTONIC, &request.time_stamp);

			send_status = mq_send(qDesTemp, (const char*) &request,
					sizeof(request), 1);
			if (send_status < 0) {
				//perror("MAIN_THREAD: exit request");
			} else {
				INFO_STDOUT("EXIT request to Temp thread sent\n");
			}

			/*Request status from Logger thread*/
			log_message_t message;
			message.req_type = TIME_TO_EXIT;
			clock_gettime(CLOCK_MONOTONIC, &message.time_stamp);
			message.log_level = CRTICAL;
			strcpy(message.thread_name, MAIN_THREAD_NAME);
			strcpy(message.message,
					"One or More thread died...Program is exiting...");

			send_status = mq_send(qDesLogger, (const char*) &message,
					sizeof(message), 1);
			if (send_status < 0) {
				//perror("MAIN_THREAD: exit request");
			} else {
				INFO_STDOUT("EXIT request to Logger thread sent\n");
			}

			heartbeat = 0;
			destroy_semaphores();
			stop_timer();
			mq_unlink(Q_NAME_MAIN);
			goto EXIT;

		} else {
			INFO_STDOUT("All threads are running...THANK GOD\n");
		}
	}

#ifdef LETS_EXIT
	/*Request status from light thread*/
	request.req_type = TIME_TO_EXIT;
	clock_gettime(CLOCK_MONOTONIC, &request.time_stamp);

	send_status = mq_send(qDesLight, (const char*) &request, sizeof(request),
			1);
	if (send_status < 0) {
		//perror("MAIN_THREAD: exit request");
	} else {
		INFO_STDOUT("EXIT request to Light thread sent\n");
	}

	/*Request status from temp thread*/
	request.req_type = TIME_TO_EXIT;
	clock_gettime(CLOCK_MONOTONIC, &request.time_stamp);

	send_status = mq_send(qDesTemp, (const char*) &request, sizeof(request), 1);
	if (send_status < 0) {
		//perror("MAIN_THREAD: exit request");
	} else {
		INFO_STDOUT("EXIT request to Temp thread sent\n");
	}

	/*Request status from Logger thread*/
	log_message_t message;
	message.req_type = TIME_TO_EXIT;
	clock_gettime(CLOCK_MONOTONIC, &message.time_stamp);
	message.log_level = CRTICAL;
	strcpy(message.thread_name, MAIN_THREAD_NAME);
	strcpy(message.message, "User Defined Exit...Program is exiting...");

	send_status = mq_send(qDesLogger, (const char*) &message, sizeof(message),
			1);
	if (send_status < 0) {
		//perror("MAIN_THREAD: exit request");
	} else {
		INFO_STDOUT("EXIT request to Logger thread sent\n");
	}

#endif

	pthread_join(light_sensor, NULL);
	pthread_join(temperature_sensor, NULL);
	pthread_join(logger_task, NULL);
	//pthread_join(socket_task, NULL);

	EXIT:
	INFO_STDOUT("Main thread Exiting...Bye Bye...\n");

	return 0;
}

