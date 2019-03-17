/*
 * utils.c
 *
 *  Created on: Mar 12, 2019
 *      Author: hardyk
 */

#include "../include/utils.h"

pthread_mutex_t sendMutex = PTHREAD_MUTEX_INITIALIZER;

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

	if (heartbeat_count == 10000) {
		INFO_STDOUT("Releasing Heartbeat semaphore\n");
		sem_post(&sem_heartbeat);
		heartbeat_count = 0;
	}

}

/**
 * @brief this function sets the period of the thread by adding a POSIX timer
 * and arming it to the specified period interval
 * @param period - period of the thread, spcified in ms
 * @return 0 if successful, 1 otherwise
 */
int start_timer(void) {

	INFO_STDOUT("Creating and Arming the timer\n");
	timer_t timer_id;
	struct itimerspec ts;
	struct sigevent se;

	/*Initialize the segevent structure to cause the
	 * signal to handle the event by creating a new thread*/
	se.sigev_notify = SIGEV_THREAD;
	se.sigev_value.sival_ptr = &timer_id;
	se.sigev_notify_function = timer_handler;
	se.sigev_notify_attributes = NULL;

	/*Initialize the timerspec structure for required
	 * period of 100ms*/
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
 * @brief this function creates a new posix message queue OR opens already created
 *  		to sned and receive messages between threads
 * @param qName
 * @return 0 if successful, 1 otherwise
 */
mqd_t create_posix_mq(char *qName) {

	mqd_t qDes;

	struct mq_attr attr;

	attr.mq_flags = 0;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = sizeof(log_message_t);

	/*Open a new posix queue*/
	qDes = mq_open(qName, O_CREAT | O_RDWR | O_NONBLOCK, 0666, &attr);

	if (qDes == -1) {

		perror("ERROR: mq_open\n");
		exit(1);
	}

	return qDes;
}

/**
 * @brief this function sends messages to the message queue identified by the qDes
 * @param qDes file descriptior for the posix queue
 * @param message message to be logged into the logfile
 * @return 0 if successful, 1 otherwise
 */

int send_message(mqd_t qDes, log_message_t *message) {

	int bytes_sent = 0;

	pthread_mutex_lock(&sendMutex);
	bytes_sent = mq_send(qDes, (const char*) message, sizeof(log_message_t), 0);

	if (bytes_sent < 0) {
		perror("ERROR: mq_send");
		return 1;
	}
	pthread_mutex_unlock(&sendMutex);

	return 0;
}

