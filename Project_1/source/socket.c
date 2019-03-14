/*
 * socket.c
 *
 *  Created on: Mar 14, 2019
 *      Author: hardyk
 */

#include "../include/socket.h"


#define SERVER_PORT						(2345)

/**
 * @breif this function is the thread function for the socket task
 * It accepts socket requrests from the remote client and sends collected data from
 * the light and temperature sensor task to the connected client
 * @param params thread parametrs, if any
 * @return void pointer
 */

void *run_socket(void *params){

	int socket_fd = 0;
	int client_socket_fd = 0;
	int reuse_address = 1;

	struct sockaddr_in server;
	struct sockaddr_in client;

	socklen_t client_address_len;

	/*create a new socket
	 * domain - IPv4, stream based socket, protocol - 0 single type
	 * */

	if(0 > (socket_fd = socket(AF_INET, SOCK_STREAM, 0))){
		perror("ERROR: socket");
		exit(1);
	}

	/*set socket options*/
	if(-1 == (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse_address, sizeof(reuse_address)))){
		perror("ERROR: setsocketopt");
		exit(1);
	}

	/*COnfigure server socket address*/
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(SERVER_PORT);

	/*Bind the socket*/
	if(0 > bind(socket_fd, (struct sockaddr*)&server, sizeof(server))){
		perror("ERRRO: bind");
	}

	/*Listen on the socket, maximum pending connection 1*/
	if(0 > listen(socket_fd, 1)){
		perror("ERROR: listen");
		exit(1);
	}
	else{
		printf("Listening on Port %d\n", SERVER_PORT);
	}

	client_address_len = sizeof(client);

	/*Accept a new connection*/
	if(0 > (client_socket_fd = accept(socket_fd, (struct sockaddr*)&client, &client_address_len))){
		perror("ERROR: accept");
	}else{
		printf("Connection with client established\n");
	}

	return NULL;
}
