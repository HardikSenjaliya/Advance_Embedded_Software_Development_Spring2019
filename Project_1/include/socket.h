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


/*function prototypes*/
void *run_socket(void *params);
client_request_response_t handle_client_request(int request_type, mqd_t qDesLight, mqd_t qDesTemp, mqd_t qDesSocket);
#endif /* INCLUDE_SOCKET_H_ */
