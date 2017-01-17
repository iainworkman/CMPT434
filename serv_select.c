/*
 * Name: Iain Workman
 * NSID: ipw969
 * Student No.: 11139430
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#include "calendar.h"

int main(int argc, char** argv) {

	char* parse_test;
	char buffer[256];
	int port;
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;  // will point to the results
	int listen_fd = 0;
	fd_set master_fds;
	fd_set read_fds;
	int max_fd;
	int i_fd;
	int incoming_fd;
	int bytes_read;
	struct sockaddr_storage client_address;
	socklen_t address_length;
	CalendarCommand command;

	if(argc == 1) {
		printf("No listen port provided\n");
		return 1;
	}
	
	port = strtol(argv[1], &parse_test, 0);
	if(port < 30001 || port > 40000 || *parse_test != '\0') {
		printf("Port is not a valid number (30001 - 40000)\n");
		return 1;
	}	
	
	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

	if ((status = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(1);
	}

	listen_fd = 
		socket(servinfo->ai_family, 
					 servinfo->ai_socktype, 
					 servinfo->ai_protocol);	

	if(listen_fd == -1) {
		fprintf(stderr, "Failed to obtain socket\n");
		exit(1);
	}
	
	if(bind(listen_fd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {

		fprintf(stderr, "Failed to bind socket to port\n");
		exit(1);
	}

	FD_SET(listen_fd, &master_fds);
	max_fd = listen_fd;
	freeaddrinfo(servinfo);

	if(listen(listen_fd, 10) == -1) {
		fprintf(stderr, "Failed to listen on port\n");
		exit(1);
	}

	while(1) {

		read_fds = master_fds;
		if (select(max_fd+1, &read_fds, NULL, NULL, NULL) == -1) {
			fprintf(stderr, "Failed calling select\n");
			exit(1);
		}

		for(i_fd = 0; i_fd <= max_fd; i_fd++) {
    	if (FD_ISSET(i_fd, &read_fds)) {
				if(i_fd == listen_fd) {
					address_length = sizeof client_address;
					incoming_fd = accept(listen_fd, 
															 (struct sockaddr *)&client_address,
															 &address_length);
					if(incoming_fd == -1) {
						fprintf(stderr, "Failed to accept incoming connection/n");
					}	else {
						FD_SET(incoming_fd, &master_fds);
						if(incoming_fd > max_fd) {
							max_fd = incoming_fd;
						}

						printf("Connection made!\n");
					}
				} else {
					bytes_read = recv(i_fd, (char*)&command, sizeof(CalendarCommand), 0);
					if (bytes_read == 0) {
						printf("Connection closed!\n");
						close(i_fd);
						FD_CLR(i_fd, &master_fds);
					} else if (bytes_read < 0) {
						fprintf(stderr, "Failed receiving data\n");
						close(i_fd);
						FD_CLR(i_fd, &master_fds);
					} else {
						printf("%s\n", command.username);
						printf("%d\n", command.command_code);
						PrintEntry(&command.event);
					}
				}
			}
		}
	}

	return 0;
}
