/*
 * socket.h
 *
 *  Created on: Mar 14, 2019
 *      Author: hardyk
 */

#ifndef INCLUDE_SOCKET_H_
#define INCLUDE_SOCKET_H_

#include "utils.h"
#include <netdb.h>
#include <sys/socket.h>

typedef enum {
	DAY, NIGHT
} day_or_night_t;

typedef enum {

	GET_TEMP_C,
	GET_TEMP_F,
	GET_TEMP_K,
	GET_LUX,
	GET_LIGHT_STATUS,
	CLOSE_CONNECTION

} client_request_t;

typedef struct {

	double temp;
	day_or_night_t day_or_night;

} client_request_response_t;

/*function prototypes*/
void *run_socket(void *params);
client_request_response_t handle_client_request(client_request_t request);
#endif /* INCLUDE_SOCKET_H_ */
