/*
 * main.c
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#include "../include/main.h"

//#define LETS_EXIT

int startup_request = 0;
extern pthread_mutex_t startup_mutex;

bool light_status = 0, temp_status = 0, logger_status = 0, socket_status = 0;

int8_t logger_startup_test(queue_descriptors_t *qDes) {

	int8_t success = 0;
	int8_t send_status = 0, received_bytes = 0;

	heartbeat_response_t response;

	/*Request status from Logger thread*/
	log_message_t message;
	message.req_type = STARTUP_TEST;

	send_status = mq_send(qDes->qDesLogger, (const char*) &message,
			sizeof(message), 1);
	if (send_status < 0) {
		perror("MAIN_THREAD: startup test request for logger");
	} else {
		//INFO_STDOUT("startup test request to Logger thread sent\n");
	}

	sleep(1);

	/*Read status sent by the temp thread*/
	received_bytes = mq_receive(qDes->qDesMain, (char*) &response,
			sizeof(response), 0);
	if (received_bytes < 0) {
		//perror("MAIN THREAD: reading heartbeat response");
	} else {

		pthread_mutex_lock(&startup_mutex);
		if (startup_request & 0x01) {
			//if (response.alive_status == LOGGER_THREAD_ALIVE) {
			logger_status = 1;
			success = 1;
		}
		pthread_mutex_unlock(&startup_mutex);
	}

	if (logger_status) {
		strcpy(message.message, "Logger Task Passed BIST");
	} else {
		strcpy(message.message, "Logger Task Failed BIST");
		set_led_on(USER_LED0);
	}

	send_status = mq_send(qDes->qDesLogger, (const char*) &message,
			sizeof(message), 1);
	if (send_status < 0) {
		perror("MAIN_THREAD: startup test request");
	} else {
		//INFO_STDOUT("startup test request to Logger thread sent\n");
	}

	return success;
}
int8_t light_startup_test(queue_descriptors_t *qDes) {
	int8_t success = 0;
	int8_t send_status = 0, received_bytes = 0;

	heartbeat_response_t response;

	request_t request;

	log_message_t message;

	/*Startup test for light task*/
	request.req_type = STARTUP_TEST;

	send_status = mq_send(qDes->qDesLight, (const char*) &request,
			sizeof(request), 1);
	if (send_status < 0) {
		perror("MAIN_THREAD: startup test request for light");
	} else {
		//INFO_STDOUT("startup test request to Light thread sent\n");
	}

	sleep(1);

	/*Read status sent by the light thread*/
	received_bytes = mq_receive(qDes->qDesMain, (char*) &response,
			sizeof(response), 0);
	if (received_bytes < 0) {

		//perror("MAIN THREAD: reading heartbeat response");
	} else {

		pthread_mutex_lock(&startup_mutex);
		if (startup_request & 0x02) {
			//if (response.alive_status == LIGHT_THREAD_ALIVE) {
			light_status = 1;
			success = 1;
		}
		pthread_mutex_unlock(&startup_mutex);
	}

	if (light_status) {
		strcpy(message.message, "Light Task Passed BIST");
	} else {
		strcpy(message.message, "Light Task Failed BIST");
		set_led_on(USER_LED1);
	}

	send_status = mq_send(qDes->qDesLogger, (const char*) &message,
			sizeof(message), 1);
	if (send_status < 0) {
		perror("MAIN_THREAD: startup test request");
	} else {
		//INFO_STDOUT("Heartbeat request to Logger thread sent\n");
	}

	return success;

}

int8_t temp_startup_test(queue_descriptors_t *qDes) {

	int8_t success = 0;
	int8_t send_status = 0, received_bytes = 0;

	heartbeat_response_t response;

	request_t request;
	log_message_t message;

	/*Request status from temp thread*/
	request.req_type = STARTUP_TEST;

	send_status = mq_send(qDes->qDesTemp, (const char*) &request,
			sizeof(request), 1);
	if (send_status < 0) {
		perror("MAIN_THREAD: startup test request for temp");
	} else {
		//INFO_STDOUT("startup test request to Temp thread sent\n");
	}

	sleep(1);

	/*Read status sent by the temp thread*/
	received_bytes = mq_receive(qDes->qDesMain, (char*) &response,
			sizeof(response), 0);
	if (received_bytes < 0) {
		//perror("MAIN THREAD: reading heartbeat response");
	} else {

		pthread_mutex_lock(&startup_mutex);
		if (startup_request & 0x04) {
			//if (response.alive_status == TEMP_THREAD_ALIVE) {
			temp_status = 1;
			success = 1;
		}
		pthread_mutex_unlock(&startup_mutex);
	}

	if (temp_status) {
		strcpy(message.message, "Temp Task Passed BIST");
	} else {
		strcpy(message.message, "Temp Task Failed BIST");
		set_led_on(USER_LED2);
	}

	send_status = mq_send(qDes->qDesLogger, (const char*) &message,
			sizeof(message), 1);
	if (send_status < 0) {
		perror("MAIN_THREAD: startup test request");
	} else {
		//INFO_STDOUT("Heartbeat request to Logger thread sent\n");
	}

	return success;
}

int8_t socket_startup_test(queue_descriptors_t *qDes) {

	int8_t success = 0;
	int8_t send_status = 0, received_bytes = 0;

	heartbeat_response_t response;

	log_message_t message;

	client_request_response_t c_response;
	c_response.req_type = STARTUP_TEST;
	send_status = mq_send(qDes->qDesSocket, (const char*) &c_response,
			sizeof(c_response), 1);
	if (send_status < 0) {
		perror("MAIN THREAD: startup test request for socket");
	} else {
		//INFO_STDOUT("Startup Reqeust to socket thread sent\n");
	}

	sleep(1);

	/*Read status sent by the temp thread*/
	received_bytes = mq_receive(qDes->qDesMain, (char*) &response,
			sizeof(response), 0);
	if (received_bytes < 0) {
		//perror("MAIN THREAD: reading heartbeat response");
	} else {

		pthread_mutex_lock(&startup_mutex);
		if (startup_request & 0x08) {
			//if (response.alive_status == SOCKET_THREAD_ALIVE) {
			socket_status = 1;
			success = 1;
		}
		pthread_mutex_unlock(&startup_mutex);
	}

	if (socket_status) {
		strcpy(message.message, "Socket Task Passed BIST");
	} else {
		strcpy(message.message, "Socket Task Failed BIST");
		set_led_on(USER_LED3);
	}

	send_status = mq_send(qDes->qDesLogger, (const char*) &message,
			sizeof(message), 1);
	if (send_status < 0) {
		perror("MAIN_THREAD: startup test request");
	} else {
		//INFO_STDOUT("Heartbeat request to Logger thread sent\n");
	}

	return success;
}

int8_t startup_test(queue_descriptors_t *qDes) {

	INFO_STDOUT("Starting startup tests\n");

	int success = 1;

	/*int i2c_fd = init_light_sensor();

	 int8_t read = read_id_register(i2c_fd);

	 if (read == 0x50) {

	 light_status = 1;
	 }

	 i2c_fd = init_temp_sensor();

	 int8_t write = 30;
	 write_tlow_register(i2c_fd, write);
	 read = read_tlow_register(i2c_fd);
	 if (write == read) {
	 temp_status = 1;
	 }

	 if(light_status < 1 || temp_status < 1){

	 return 0;
	 }*/

	light_status = light_startup_test(qDes);
	temp_status = temp_startup_test(qDes);
	logger_status = logger_startup_test(qDes);
	socket_status = socket_startup_test(qDes);

	if (temp_status < 1 || light_status < 1 || logger_status < 1
			|| socket_status < 1) {
		printf("Startup Test failed\n");
		return 0;
	}

	INFO_STDOUT("Startup Test Completed Successfully\n");

	return success;
}

/**
 * @brief main function
 * @return 0
 */

int main(int argc, char **argv) {

	int heartbeat = 1;

	pthread_t light_sensor, temperature_sensor, logger_task, socket_task;

	log_message_t msg;
	request_t request;
	heartbeat_response_t response;

	logfile_attr_t logfile;

	int send_status = 0, received_bytes = 0;

	if (argc < 2) {
		printf(
				"Invalid Commnad Line Arguments/No Arguments. Please provide required arguments"
						"Usage: /filepath/filename.txt");
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
	mqd_t qDesSocket = create_socket_mq();

	queue_descriptors_t obj;

	obj.qDesLight = qDesLight;
	obj.qDesLogger = qDesLogger;
	obj.qDesMain = qDesMain;
	obj.qDesTemp = qDesTemp;
	obj.qDesSocket = qDesSocket;

	//send_message(Q_LOGGER_ID, "ERROR: creating light sensor thread\n", L_ERROR, P_ERROR, qDesLogger);

	/*Initializes user leds*/
	init_leds();

	/*Spwan new threads*/
	//INFO_STDOUT("Creating new threads\n");
	/*1. light sensor thread*/
	if (pthread_create(&light_sensor, NULL, run_light_sensor, NULL)) {
		ERROR_STDOUT("ERROR: creating light sensor thread\n");
		send_message(Q_LOGGER_ID, "ERROR: creating light sensor thread\n",
				L_ERROR, P_ERROR, qDesLogger);
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

	/*Start timer for periodic execution of temp and
	 * light threads*/
	if (start_timer()) {
		ERROR_STDOUT("ERROR: starting timer");
		exit(1);
	}

	sleep(1);

	int status = startup_test(&obj);

	if (!status) {
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
		//INFO_STDOUT("MAIN_THREAD: message to logger sent\n");
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

			/*Blink LEDs to indicate an error*/
			set_led_on(USER_LED0);
			set_led_on(USER_LED1);
			set_led_on(USER_LED2);
			set_led_on(USER_LED3);

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
			message.log_level = L_CRTICAL;
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
	message.log_level = L_CRTICAL;
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
