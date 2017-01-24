/*
 * Name: Iain Workman
 * NSID: ipw969
 * Student No.: 11139430
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "calendar.h"

int main(int argc, char** argv) {

	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo; 
	int socket_fd;
	CalendarCommand command;
	CalendarResponse response;
	int parse_status;
	int port;

	/* Parse Arguments */
	parse_status = ParseCommand(argc, argv, &command);
	if(parse_status == -1) {
		fprintf(stderr, "Failed to parse provided arguments\n");
		exit(1);
	}	

	port = strtol(argv[2], 0, 10);
	if(port < 30001 || port > 40000) {
		fprintf(stderr, "Port is not valid (30001 - 40000)\n");
		exit(1);
	}		

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;   
	hints.ai_socktype = SOCK_STREAM;

	if ((status = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
    fprintf(stderr, "Failed to get address info\n");
    exit(1);
	}

	socket_fd = socket(
								servinfo->ai_family, 
								servinfo->ai_socktype, 
								servinfo->ai_protocol);

	if(socket_fd == -1) {
		fprintf(stderr, "Failed to obtain socket\n");
		exit(1);
	}
	status = connect(socket_fd, servinfo->ai_addr, servinfo->ai_addrlen);
	
	if(status == -1) {
		fprintf(stderr, "Failed to connect to %s:%s\n", argv[1], argv[2]);
		exit(1);
	}
	send(socket_fd, (char*)&command, sizeof(CalendarCommand), 0);
	recv(socket_fd, (char*)&response, sizeof(CalendarResponse), 0);
	
	PrintError(response.response_code);
	if(response.response_code == ADD_SUCCESS) {
		printf("Entry added successfully\n");
	} else if(response.response_code == REMOVE_SUCCESS) {
		printf("Entry removed successfully\n");
	} if(response.response_code == GET) {
		while(response.response_code != GET_END) {
			PrintEntry(&response.entry);
			recv(socket_fd, (char*)&response, sizeof(CalendarResponse), 0);
		}
	}

	freeaddrinfo(servinfo);

	return 0;
}
