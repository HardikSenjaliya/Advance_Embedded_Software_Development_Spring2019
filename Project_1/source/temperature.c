/*
 * temperature.c
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#include "../include/temperature.h"

/**
 * @brief this function is the thread function for the thread temperature_sensor
 * @param params
 * @return NULL
 */
void *run_temperature_sensor(void *params) {

	INFO_STDOUT("Temperature Sensor thread started running...\n");

	mqd_t qDesTemp = create_posix_mq(Q_NAME_TEMP);
	mqd_t qDesLogger = create_posix_mq(Q_NAME_LOGGER);
	mqd_t qDesMain = create_posix_mq(Q_NAME_MAIN);

	log_message_t msg, request, response;
	int send_status = 0, received_bytes = 0;

	msg.log_level = 0;
	strcpy(msg.message, "Hello From Temp Sensor task");
	strcpy(msg.thread_name, TEMP_THREAD_NAME);
	clock_gettime(CLOCK_MONOTONIC, &msg.time_stamp);

	send_message(qDesLogger, &msg);

	while (1) {
		//INFO_STDOUT("Waiting for Temp semaphore\n");
		sem_wait(&sem_temp);
		//	INFO_STDOUT("sem_temp released...started executing...\n");
		send_message(qDesLogger, &msg);

		received_bytes = mq_receive(qDesTemp, (char*) &request, sizeof(request),
				0);
		if (received_bytes < 0) {
			//ERROR_STDOUT("ERROR: while reading responses from the LIGHT Q\n");
		}

		if (request.req_type == SEND_ALIVE_STATUS) {
			strcpy(response.thread_name, TEMP_THREAD_NAME);
			response.alive_status = TEMP_THREAD_ALIVE;

			send_status = mq_send(qDesMain, (const char*) &response,
					sizeof(response), 1);
			if (send_status < 0) {
				//ERROR_STDOUT("ERROR: while sending responses to the MAIN Q from LIGHT THREAD\n");
			}

			request.req_type = 0;
		}
		if (request.req_type == TIME_TO_EXIT) {
			goto EXIT;
		}

	}

	EXIT:
	return NULL;

}
