/*
 * temperature.c
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#include "../include/temperature.h"
#include "../include/TMP_102_temp_sensor.h"

/**
 * @brief this function is the thread function for the thread temperature_sensor
 * @param params
 * @return NULL
 */
void *run_temperature_sensor(void *params) {

	INFO_STDOUT("Temperature Sensor thread started running...\n");

	/*Create a POSIX message queue to send requests to child threads*/
	mqd_t qDesTemp = create_temp_mq();
	mqd_t qDesMain = create_main_mq();
	mqd_t qDesLogger = create_logger_mq();

	log_message_t msg;
	request_type_t request;
	heartbeat_response_t response;

	int send_status = 0, received_bytes = 0;
	double temperature = 0;

	msg.log_level = 0;
	strcpy(msg.thread_name, TEMP_THREAD_NAME);
	clock_gettime(CLOCK_MONOTONIC, &msg.time_stamp);

	strcpy(msg.message, " Hello From Temp Sensor task");

	int i2c_fd = init_temp_sensor();


	send_status = mq_send(qDesLogger, (const char*) &msg, sizeof(msg), 0);
	if (send_status < 0) {
		//perror("TEMP THREAD");
	} else {
		//INFO_STDOUT("TEMP_THREAD: message to logger sent\n");
	}

	while (1) {

		sem_wait(&sem_temp);

		received_bytes = mq_receive(qDesTemp, (char*) &request, sizeof(request),
				0);
		if (received_bytes < 0) {
			//perror("TEMP THREAD");
		} else {

			switch (request.req_type) {
			case SEND_ALIVE_STATUS:
				strcpy(response.thread_name, TEMP_THREAD_NAME);
				response.alive_status = TEMP_THREAD_ALIVE;

				send_status = mq_send(qDesMain, (const char*) &response,
						sizeof(response), 1);
				if (send_status < 0) {
					//perror("TEMP THREAD");
				} else {
					//INFO_STDOUT("TEMP_THREAD: Response to hearbeat request sent\n");
					request.req_type = 0;
				}
				break;

			case TIME_TO_EXIT:
				INFO_STDOUT("TEMP_THREAD: Request to Exit received\n");
				goto EXIT;
				break;
			default:
				break;

			}
		}

		/*TODO send log message to logger*/
		temperature = read_temperature_register(i2c_fd);
		msg.log_level = L_INFO;
		strcpy(msg.thread_name, TEMP_THREAD_NAME);
		clock_gettime(CLOCK_MONOTONIC,  &msg.time_stamp);
		snprintf(msg.message, MESSAGE_SIZE, "%s -> %f", "Current Temperature Reading", temperature);

		send_status = mq_send(qDesLogger, (const char*) &msg, sizeof(msg), P_INFO);
		if(send_status < 0){
			perror("ERROR: sending temperature read log");
		}
		//printf("Temperature read - %f\n", temperature);

	}

	EXIT:

	mq_unlink(Q_NAME_TEMP);
	INFO_STDOUT("TEMP_THREAD...EXITING\n");

	return NULL;
}
