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

	/* Parse Arguments */
	parse_status = ParseCommand(argc, argv, &command);
	if(parse_status == -1) {
		fprintf(stderr, "Failed to parse provided arguments\n");
		exit(1);
	}	

	printf("%s\n", command.username);
	printf("%d\n", command.command_code);
	PrintEntry(&command.event);
	
	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

	if ((status = getaddrinfo("127.0.0.1", "30001", &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(1);
	}

	socket_fd = socket(
								servinfo->ai_family, 
								servinfo->ai_socktype, 
								servinfo->ai_protocol);

	connect(socket_fd, servinfo->ai_addr, servinfo->ai_addrlen);
	
	send(socket_fd, (char*)&command, sizeof(CalendarCommand), 0);
	recv(socket_fd, (char*)&response, sizeof(CalendarResponse), 0);
	
	if(response.response_code == ERR) {
		fprintf(stderr, "Error\n");
	} else if(response.response_code == ADD_SUCCESS) {
		printf("Entry added successfully\n");
	} else if(response.response_code == REMOVE_SUCCESS) {
		printf("Entry removed successfully\n");
	}

	freeaddrinfo(servinfo); // free the linked-list

	return 0;
}
