/*
 * light.c
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#include "../include/light.h"


extern int heartbeat_request;

/**
 * @brief this function is the thread function for the thread light_sensor
 * @param params
 * @return NULL
 */
void *run_light_sensor(void *params) {

	INFO_STDOUT("Light sensor thread started running...\n");

	mqd_t qDes = create_posix_mq(Q_NAME_LIGHT);
	mqd_t qDesHB = create_posix_mq(Q_NAME_HB);

	int send_status = 0, bytes_recevied = 0;

	log_message_t msg, request, response;
	msg.log_level = 0;
	strcpy(msg.message, "Hello From Light Sensor task");
	strcpy(msg.thread_name, LIGHT_THREAD_NAME);
	clock_gettime(CLOCK_MONOTONIC, &msg.time_stamp);
	send_message(qDes, &msg);

	while (1) {
		sem_wait(&sem_light);
		send_message(qDes, &msg);

		//printf("Heartbeat Request flag = %d\n", heartbeat_request);

		if (heartbeat_request) {
		//	INFO_STDOUT("Heartbeat request flag is set\n");
			bytes_recevied = mq_receive(qDesHB, (char*) &request,
					sizeof(request), 0);
			if (bytes_recevied < 0) {
				ERROR_STDOUT("ERROR: while reading heartbeat request\n");
			}

			if (request.req_type == SEND_ALIVE_STATUS) {
				INFO_STDOUT("Request for heartbeat received\n");
				response.alive_status = LIGHT_THREAD_ALIVE;
				strcpy(response.thread_name, LIGHT_THREAD_NAME);

				for (int i = 0; i < 3; i++) {
					send_status = mq_send(qDesHB, (const char*) &response,
							sizeof(response), 2);
					if (send_status < 0) {
						ERROR_STDOUT(
								"ERROR: while sending heartbeat request's response\n");
					}
					sleep(1);
				}
			}
		}

	}

	return NULL;
}
