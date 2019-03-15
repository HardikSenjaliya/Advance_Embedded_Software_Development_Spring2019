/*
 * socket.c
 *
 *  Created on: Mar 14, 2019
 *      Author: hardyk
 */

#include "../include/socket.h"

#define SERVER_PORT						(2345)

int request_received = 0;

/**
 * @brief this function handles requests recevied from the remote socket
 * @param request type of request from the remote client
 * @return response to be sent
 */

client_request_response_t handle_client_request(client_request_t request) {

	client_request_response_t response = { 0 };

	switch (request) {

	case GET_TEMP_C: {
		break;
	}

	case GET_TEMP_F: {
		break;
	}
	case GET_TEMP_K: {
		break;
	}
	case GET_LUX: {
		break;
	}
	case GET_LIGHT_STATUS: {
		break;
	}
	case CLOSE_CONNECTION: {
		break;
	}

	default: {
		ERROR_STDOUT("Invalid Request from client\n");
		break;
	}

	}

	return response;
}

/**
 * @brief this function is the thread function for the socket task
 * It accepts socket requrests from the remote client and sends collected data from
 * the light and temperature sensor task to the connected client
 * @param params thread parametrs, if any
 * @return void pointer
 */

void *run_socket(void *params) {

	INFO_STDOUT("Socket Task started running\n");

	client_request_t request = 0;
	client_request_response_t response = { 0 };

	int bytes_read = 0;

	int socket_fd = 0;
	int client_socket_fd = 0;
	int reuse_address = 1;

	struct sockaddr_in server;
	struct sockaddr_in client;

	socklen_t client_address_len;

	/*create a new socket
	 * domain - IPv4, stream based socket, protocol - 0 single type
	 * */

	if (0 > (socket_fd = socket(AF_INET, SOCK_STREAM, 0))) {
		perror("ERROR: socket");
		exit(1);
	}

	/*set socket options*/
	if (-1
			== (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse_address,
					sizeof(reuse_address)))) {
		perror("ERROR: setsocketopt");
		exit(1);
	}

	/*COnfigure server socket address*/
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(SERVER_PORT);

	/*Bind the socket*/
	if (0 > bind(socket_fd, (struct sockaddr*) &server, sizeof(server))) {
		perror("ERROR: bind");
		INFO_STDOUT("Exiting...\n");
		exit(1);
	}

	/*Listen on the socket, maximum pending connection 1*/
	if (0 > listen(socket_fd, 1)) {
		perror("ERROR: listen");
		INFO_STDOUT("Exiting...\n");
		exit(1);
	} else {
		INFO_STDOUT("Server Started Listening on the port\n");
	}

	client_address_len = sizeof(client);

	/*Accept a new connection*/
	if (0
			> (client_socket_fd = accept(socket_fd, (struct sockaddr*) &client,
					&client_address_len))) {
		perror("ERROR: accept");
		INFO_STDOUT("Exiting...");
		exit(1);
	} else {
		INFO_STDOUT("Connection with client established\n");
		request_received = 1;
	}

	if (request_received) {

		bytes_read = read(client_socket_fd, (char*) request, sizeof(request));
		if (bytes_read < 0) {
			ERROR_STDOUT("ERROR: reading request from client\n");
		} else {
			response = handle_client_request(request);
			if (-1
					== send(client_socket_fd, (void*) &response,
							sizeof(response), 0)) {
				ERROR_STDOUT("ERROR: sending response to the client");
			}
		}
	}

	return NULL;
}
