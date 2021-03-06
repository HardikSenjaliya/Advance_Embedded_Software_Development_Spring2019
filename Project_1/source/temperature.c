/*
 * temperature.c
 *	@brief this file is a thread function of the temperature sensor thread.
 *	It handles all the requests and provides required response.
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#include "../include/temperature.h"
#include "../include/TMP_102_temp_sensor.h"

extern int startup_request;
extern pthread_mutex_t startup_mutex;
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
	mqd_t qDesSocket = create_socket_mq();

	log_message_t msg;
	request_t request;
	heartbeat_response_t response;
	client_request_response_t client_response;

	int send_status = 0, received_bytes = 0;
	double temperature = 0;

	struct pollfd fdset[1];
	char value[4];

	int gpio120_fd = open("/sys/class/gpio/gpio60/value",
	O_RDONLY);
	if (gpio120_fd < 0) {
		send_message(Q_LOGGER_ID, "ERROR: opening GPIO 120\n", L_CRTICAL,
				P_CRITICAL, qDesLogger);
	}

	fdset[0].fd = gpio120_fd;
	fdset[0].events = POLLPRI;

	int n = read(gpio120_fd, &value, sizeof(value));
	if (n > 0) {
		printf("Initial value %c\n", value[0]);
		lseek(gpio120_fd, 0, SEEK_SET);
	}

	msg.log_level = 0;
	strcpy(msg.thread_name, TEMP_THREAD_NAME);
	clock_gettime(CLOCK_MONOTONIC, &msg.time_stamp);

	strcpy(msg.message, " Hello From Temp Sensor task");

	int i2c_fd = init_temp_sensor();

	extra_credit_temp(i2c_fd);

	send_status = mq_send(qDesLogger, (const char*) &msg, sizeof(msg), 0);
	if (send_status < 0) {
		//perror("TEMP THREAD");
	} else {
		//INFO_STDOUT("TEMP_THREAD: message to logger sent\n");
	}

	while (1) {

		sem_wait(&sem_temp);

		temperature = read_temperature_register(i2c_fd);
		msg.log_level = L_INFO;
		strcpy(msg.thread_name, TEMP_THREAD_NAME);
		clock_gettime(CLOCK_MONOTONIC, &msg.time_stamp);
		snprintf(msg.message, MESSAGE_SIZE, "%s -> %f",
				"Current Temperature Reading", temperature);

		send_status = mq_send(qDesLogger, (const char*) &msg, sizeof(msg),
				P_INFO);
		if (send_status < 0) {
			//perror("ERROR: sending temperature read log");
		}

		// printf("Temperature read - %f\n", temperature);

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
						sizeof(response), P_WARNING);
				if (send_status < 0) {
					//perror("TEMP THREAD");
				} else {
					request.req_type = 0;
				}
				break;

			case GET_TEMP_C:
				client_response.data = read_temperature_register(i2c_fd);
				strcpy(client_response.message, "Requested Temp in C");

				send_status = mq_send(qDesSocket,
						(const char*) &client_response, sizeof(client_response),
						P_INFO);
				if (send_status < 0) {
					//perror("Sending Response to Socket");
				} else {
					request.req_type = 0;
				}

				break;
			case GET_TEMP_F:
				client_response.data = convert_temp_farenheit(i2c_fd);
				strcpy(client_response.message, "Requested Temp in Farenheit");

				send_status = mq_send(qDesSocket,
						(const char*) &client_response, sizeof(client_response),
						P_INFO);
				if (send_status < 0) {
					//perror("Sending Response to Socket");
				} else {
					request.req_type = 0;
				}

				break;
			case GET_TEMP_K:
				client_response.data = convert_temp_kelvin(i2c_fd);
				strcpy(client_response.message, "Requested Temp in Kelvin");

				send_status = mq_send(qDesSocket,
						(const char*) &client_response, sizeof(client_response),
						P_INFO);
				if (send_status < 0) {
					//perror("Sending Response to Socket");
				} else {
					request.req_type = 0;
				}

				break;

			case STARTUP_TEST: {

				int8_t write = 30;
				write_tlow_register(i2c_fd, write);
				int8_t read = read_tlow_register(i2c_fd);
				if (write == read) {
					response.alive_status = TEMP_THREAD_ALIVE;

					pthread_mutex_lock(&startup_mutex);
					startup_request |= (1 << 2);
					pthread_mutex_unlock(&startup_mutex);

				}

				send_status = mq_send(qDesMain, (const char*) &response,
						sizeof(response), 1);
				if (send_status < 0) {
					//perror("TEMP THREAD");
				} else {
					//INFO_STDOUT("TEMP_THREAD: Response to hearbeat request sent\n");
					request.req_type = 0;
				}
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

		/*Handle the interrupt*/
		int rc = poll(fdset, 1, 50);

		if (rc < 0) {
			printf("Poll failed\n");
		}

		if (rc > 0) {

			if (fdset[0].revents & POLLPRI) {
				n = read(gpio120_fd, &value, sizeof(value));
			//	printf("Current temp is %f New value %c\n", temperature, value[0]);

				printf("Temp Sensor interrupt occoured\n");
				lseek(gpio120_fd, 0, SEEK_SET);

			}
		}
	}

	EXIT:

	mq_unlink(Q_NAME_TEMP);
	close(i2c_fd);
	INFO_STDOUT("TEMP_THREAD...EXITING\n");

	return NULL;
}
