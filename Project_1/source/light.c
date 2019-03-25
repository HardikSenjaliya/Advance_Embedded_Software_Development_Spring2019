/*
 * light.c
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#include "../include/light.h"

/**
 * @brief this function is the thread function for the thread light_sensor
 * @param params
 * @return NULL
 */
void *run_light_sensor(void *params) {

	INFO_STDOUT("Light sensor thread started running...\n");

	/*Create a POSIX message queue to send requests to child threads*/
	mqd_t qDesLight = create_light_mq();
	mqd_t qDesLogger = create_logger_mq();
	mqd_t qDesMain = create_main_mq();

	log_message_t msg;
	request_type_t request;
	heartbeat_response_t response;

	msg.log_level = 0;
	strcpy(msg.thread_name, LIGHT_THREAD_NAME);
	clock_gettime(CLOCK_MONOTONIC, &msg.time_stamp);
	strcpy(msg.message, " Hello From Light Sensor task");

	int send_status = 0, received_bytes = 0;

	while (1) {

		sem_wait(&sem_light);

		send_status = mq_send(qDesLogger, (const char*) &msg, sizeof(msg), 0);
		if (send_status < 0) {
			//perror("LIGHT THREAD");
		} else {
			//INFO_STDOUT("LIGHT_THREAD: message to logger sent\n");
		}

		received_bytes = mq_receive(qDesLight, (char*) &request,
				sizeof(request), 0);
		if (received_bytes < 0) {
			//perror("LIGHT THREAD: mq_receive");
		} else {

			switch (request.req_type) {
			case SEND_ALIVE_STATUS:
				strcpy(response.thread_name, LIGHT_THREAD_NAME);
				response.alive_status = LIGHT_THREAD_ALIVE;

				send_status = mq_send(qDesMain, (const char*) &response,
						sizeof(response), 1);
				if (send_status < 0) {
					perror("LIGHT THREAD: sending heartbeat response");
				} else {
					//INFO_STDOUT("Light-response to hearbeat request sent\n");
					request.req_type = 0;
				}

				break;

			case TIME_TO_EXIT:
				INFO_STDOUT("LIGHT_THREAD: Request to Exit received\n");
				goto EXIT;
				break;
			default:
				break;
			}
		}
	}

	EXIT:
	mq_unlink(Q_NAME_LIGHT);
	INFO_STDOUT("LIGHT_THREAD...EXITING\n");

	return NULL;
}
