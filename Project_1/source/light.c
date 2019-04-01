/*
 * light.c
 *
 *  @brief this file is the thread function for light sensor. It responds to all
 *  the requests from the main and socket task with logging data into the logfile by sending
 *  messages to the loggeer task
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#include "../include/light.h"
#include <poll.h>

extern int startup_request;
extern pthread_mutex_t startup_mutex;
/**
 * @brief this function is the thread function for the thread light_sensor
 * @param params
 * @return NULL
 */
void *run_light_sensor(void *params) {

	/*Create a POSIX message queue to send requests to child threads*/
	mqd_t qDesLight = create_light_mq();
	mqd_t qDesLogger = create_logger_mq();
	mqd_t qDesMain = create_main_mq();
	mqd_t qDesSocket = create_socket_mq();

	send_message(Q_LOGGER_ID, "INFO: Light Sensor Thread Started Running\n",
			L_INFO, P_INFO, qDesLogger);

	log_message_t msg;
	request_t request;
	heartbeat_response_t response;
	client_request_response_t client_response;

	static bool prev_state = NIGHT;
	bool curr_state = NIGHT;

	struct pollfd fdset[1];
	char value[4];

	int gpio121_fd = open("/sys/class/gpio/gpio66/value",
	O_RDONLY);
	if (gpio121_fd < 0) {
		send_message(Q_LOGGER_ID, "ERROR: opening GPIO 120\n", L_CRTICAL,
				P_CRITICAL, qDesLogger);
	}

	fdset[0].fd = gpio121_fd;
	fdset[0].events = POLLPRI;

	int n = read(gpio121_fd, &value, sizeof(value));
	if (n > 0) {
		printf("Initial value %c\n", value[0]);
		lseek(gpio121_fd, 0, SEEK_SET);
	}

	strcpy(msg.thread_name, LIGHT_THREAD_NAME);
	clock_gettime(CLOCK_MONOTONIC, &msg.time_stamp);
	strcpy(msg.message, " Hello From Light Sensor task");

	int send_status = 0, received_bytes = 0;

	int i2c_fd = init_light_sensor();

	extra_credit_light(i2c_fd);

	send_status = mq_send(qDesLogger, (const char*) &msg, sizeof(msg), 0);
	if (send_status < 0) {
		//perror("LIGHT THREAD");
	} else {
		//INFO_STDOUT("LIGHT_THREAD: message to logger sent\n");
	}

	while (1) {

		sem_wait(&sem_light);

		double lux_value = read_lux_data(i2c_fd);
		curr_state = day_or_night(lux_value);

		if (curr_state != prev_state) {
			printf("State Chnaged\n");
			prev_state = curr_state;
			snprintf(msg.message, MESSAGE_SIZE, "%s %d",
					"** State Changed ** Current State(0 = Day; 1 = NIGHT) = ",
					curr_state);
		} else {
			snprintf(msg.message, MESSAGE_SIZE, "%s -> %f",
					"Current Lux Reading", lux_value);
		}

		msg.log_level = L_INFO;
		strcpy(msg.thread_name, LIGHT_THREAD_NAME);
		clock_gettime(CLOCK_MONOTONIC, &msg.time_stamp);

		send_status = mq_send(qDesLogger, (const char*) &msg, sizeof(msg),
				P_INFO);
		if (send_status < 0) {
			//perror("ERROR: sending lux read log");
		}

		/*Read any received request from other tasks and act accordingly*/
		received_bytes = mq_receive(qDesLight, (char*) &request,
				sizeof(request), 0);
		if (received_bytes < 0) {
		} else {

			switch (request.req_type) {
			case SEND_ALIVE_STATUS:
				strcpy(response.thread_name, LIGHT_THREAD_NAME);
				response.alive_status = LIGHT_THREAD_ALIVE;

				send_status = mq_send(qDesMain, (const char*) &response,
						sizeof(response), P_WARNING);
				if (send_status < 0) {
					//perror("LIGHT THREAD: sending heartbeat response");
				} else {
					request.req_type = 0;
				}

				break;

			case GET_LUX:
				strcpy(client_response.message, "Requested Lux Data");

				send_status = mq_send(qDesSocket,
						(const char*) &client_response, sizeof(client_response),
						P_INFO);
				if (send_status < 0) {
					//perror("LIGHT THREAD: sending socket response");
				} else {
					request.req_type = 0;
				}

				break;

			case GET_LIGHT_STATUS:
				strcpy(client_response.message, "Requested Light Data");
				client_response.data = curr_state;

				send_status = mq_send(qDesSocket,
						(const char*) &client_response, sizeof(client_response),
						P_INFO);
				if (send_status < 0) {
					//perror("LIGHT THREAD: sending socket response");
				} else {
					request.req_type = 0;
				}

				break;

			case STARTUP_TEST: {

				int8_t read = read_id_register(i2c_fd);

				if (read == 0x50) {

					pthread_mutex_lock(&startup_mutex);
					startup_request |= (1 << 1);
					pthread_mutex_unlock(&startup_mutex);
				}
				send_status = mq_send(qDesMain, (const char*) &response,
						sizeof(response), 1);
				if (send_status < 0) {
					perror("LIGHT THREAD: sending heartbeat response");
				} else {
					request.req_type = 0;
				}
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

		/*Handle the interrupt*/
		int rc = poll(fdset, 1, 50);

		if (rc < 0) {
			printf("Poll failed\n");
		}

		if (rc > 0) {

			if (fdset[0].revents & POLLPRI) {
				n = read(gpio121_fd, &value, sizeof(value));
				printf("New value %c\n", value[0]);
				//printf("GPIO 120 interrupt occoured");
				lseek(gpio121_fd, 0, SEEK_SET);
				clear_interrupt(i2c_fd);
			}
		}
	}

	EXIT: mq_unlink(Q_NAME_LIGHT);
	close(i2c_fd);
	INFO_STDOUT("LIGHT_THREAD...EXITING\n");

	return NULL;
}
