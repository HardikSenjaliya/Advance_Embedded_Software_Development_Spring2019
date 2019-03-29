/*
 * utils.c
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#include "../include/utils.h"

pthread_mutex_t sendMutex = PTHREAD_MUTEX_INITIALIZER;
timer_t timer_id;

/**
 * @brief this function initializes all the semaphores to be used in the
 * program at global level
 * @return 0 if succesful, 1 otherwise
 */
int initialize_semaphores(void) {

	INFO_STDOUT("initializing Semaphores\n");
	if (-1 == sem_init(&sem_light, 0, 0)) {
		ERROR_STDOUT("ERROR: initializing semaphore\n");
		return 1;
	}

	if (-1 == sem_init(&sem_temp, 0, 0)) {
		ERROR_STDOUT("ERROR: initializing semaphore\n");
		return 1;
	}

	if (-1 == sem_init(&sem_heartbeat, 0, 0)) {
		ERROR_STDOUT("ERROR: initializing semaphore\n");
		return 1;
	}

	return 0;
}

int destroy_semaphores(void){

	sem_destroy(&sem_light);
	sem_destroy(&sem_light);
	sem_destroy(&sem_heartbeat);

	return 0;

}

/**
 * @brief this function executes whenever timer expires
 * and releaes semaphores for light and temp threads. Light sensor thread
 * will run every 10 ms and temperature sensor thread will run every 25ms
 * @param arg
 * @return void
 */
void timer_handler(union sigval arg) {

	//INFO_STDOUT("Timer expired, Running Handler\n");
	static int count = 1;
	//printf("Counter - %d\n", count);

	static int heartbeat_count = 0;

	heartbeat_count += 1;

	count = (count + 1) % 50;

	if ((count % 10) == 1) {
		//INFO_STDOUT("Releasing Light Semaphore\n");
		sem_post(&sem_light);
	}

	if ((count % 25) == 0) {
		//INFO_STDOUT("Releasing Temp Semaphores\n");
		sem_post(&sem_temp);
	}

	if (heartbeat_count == 5000) {
		//INFO_STDOUT("Releasing Heartbeat semaphore\n");
		sem_post(&sem_heartbeat);
		heartbeat_count = 0;
	}

}

/**
 * @brief this function deletes the timer in case of error or exit
 * @return 0 if succesfull, 1 otherwise
 */
int stop_timer() {

	if (-1 == timer_delete(timer_id)) {
		ERROR_STDOUT("ERRRO: while deleting the timer\n");
	} else {
		INFO_STDOUT("Timer deleted\n");
	}

	return 0;
}

/**
 * @brief this function sets the period of the thread by adding a POSIX timer
 * and arming it to the specified period interval
 * @return 0 if successful, 1 otherwise
 */
int start_timer(void) {

	INFO_STDOUT("Creating and Arming the timer\n");

	struct itimerspec ts;
	struct sigevent se;

	/*Initialize the segevent structure to cause the
	 * signal to handle the event by creating a new thread*/
	se.sigev_notify = SIGEV_THREAD;
	se.sigev_value.sival_ptr = &timer_id;
	se.sigev_notify_function = timer_handler;
	se.sigev_notify_attributes = NULL;

	/*Initialize the timerspec structure for required
	 * period of 1ms*/
	long long int nanoSec = 1 * MSEC_TO_NSEC;
	ts.it_value.tv_sec = nanoSec / NSEC_TO_SEC;
	ts.it_value.tv_nsec = nanoSec % NSEC_TO_SEC;
	ts.it_interval.tv_sec = ts.it_value.tv_sec;
	ts.it_interval.tv_nsec = ts.it_value.tv_nsec;

	/*create a new timer*/
	if ((timer_create(CLOCK_MONOTONIC, &se, &timer_id)) == -1) {
		ERROR_STDOUT("ERROR - creating a new timer failed\n");
		return 1;
	}

	/*Set the timer to be periodic at 100msec*/
	if ((timer_settime(timer_id, 0, &ts, 0)) == -1) {
		ERROR_STDOUT("ERROR - setting up a period of the timer\n");
		return 1;
	}

	INFO_STDOUT("Timer Setting Done\n");

	return 0;
}

/**
 * @brief this function creates a POSIX message queue for the light
 * thread. The purpose of this queue is to receive any messages related to the light thread and
 * send log messages to the logger thread via logger message queue
 * @return message queue file descriptor
 */
mqd_t create_light_mq(void) {

	mqd_t qDes;

	struct mq_attr attr;

	attr.mq_flags = 0;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = sizeof(request_t);

	/*Open a new posix queue*/
	qDes = mq_open(Q_NAME_LIGHT, O_CREAT | O_RDWR | O_NONBLOCK , 0666, &attr);

	if (qDes == -1) {

		perror("ERROR: mq_open\n");
		exit(1);
	}

	return qDes;
}

/**
 * @brief this function creates a POSIX message queue for the temp
 * thread. The purpose of this queue is to receive any messages related to the temp thread
 * and send log messages to logger thread via logger queue
 * @return message queue file descriptor
 */
mqd_t create_temp_mq(void) {
	mqd_t qDes;

	struct mq_attr attr;

	attr.mq_flags = 0;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = sizeof(request_t);

	/*Open a new posix queue*/
	qDes = mq_open(Q_NAME_TEMP, O_CREAT | O_RDWR | O_NONBLOCK , 0666, &attr);

	if (qDes == -1) {

		perror("ERROR: mq_open\n");
		exit(1);
	}

	return qDes;
}

/**
 * @brief this function creates a POSIX message queue for the logger thread
 * thread. The purpose of this queue is to receive messages related from all the thread
 * to log into the logfile
 * @return message queue file descriptor
 */
mqd_t create_logger_mq(void) {
	mqd_t qDes;

	struct mq_attr attr;

	attr.mq_flags = 0;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = sizeof(log_message_t);

	/*Open a new posix queue*/
	qDes = mq_open(Q_NAME_LOGGER, O_CREAT | O_RDWR | O_NONBLOCK, 0666, &attr);

	if (qDes == -1) {

		perror("ERROR: mq_open\n");
		exit(1);
	}

	return qDes;
}

/**
 * @brief this function creates a POSIX message queue for the main
 * thread. The purpose of this queue is to receive messages sent by all the child threads when
 * heartbeat status is requested
 * @return 0 if succesful, 1 otherwise
 */
mqd_t create_main_mq(void) {

	mqd_t qDes;

	struct mq_attr attr;

	attr.mq_flags = 0;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = sizeof(heartbeat_response_t);

	/*Open a new posix queue*/
	qDes = mq_open(Q_NAME_MAIN, O_CREAT | O_RDWR, 0666, &attr);

	if (qDes == -1) {

		perror("ERROR: mq_open\n");
		exit(1);
	}

	return qDes;
}

/**
 * @brief this function creates a POSIX message queue for the main
 * thread. The purpose of this queue is to receive messages sent by all the child threads when
 * heartbeat status is requested
 * @return 0 if succesful, 1 otherwise
 */
mqd_t create_socket_mq(void) {

	mqd_t qDes;

	struct mq_attr attr;

	attr.mq_flags = 0;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = sizeof(client_request_response_t);

	/*Open a new posix queue*/
	qDes = mq_open(Q_NAME_SOCKET, O_CREAT | O_RDWR, 0666, &attr);

	if (qDes == -1) {

		perror("ERROR: mq_open\n");
		exit(1);
	}

	return qDes;
}


























