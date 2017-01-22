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
	int port;
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;  // will point to the results
	int listen_fd = 0;
	int incoming_fd;
	int bytes_read;
	struct sockaddr_storage client_address;
	socklen_t address_length;
	CalendarCommand command;
	CalendarEntry* temp_entry;
	Calendar* get_returns = 0;
	CalendarResponse response;
	int command_status;
	int yes=1;
	int calendar_pid;

	if(argc == 1) {
		fprintf(stderr, "No listen port provided\n");
		exit(1);
	}
	
	port = strtol(argv[1], &parse_test, 0);
	if(port < 30001 || port > 40000 || *parse_test != '\0') {
		fprintf(stderr, "Port is not valid (30001 - 40000)\n");
		exit(1);
	}	
	

	/* Spin up the calendar subprocess */
	calendar_pid = fork();
	if(calendar_pid < 0) {
		fprintf(stderr, "Error creating calendar process.\n");
		exit(1);
	}	else if (calendar_pid == 0) {
		/* Child process, exec() the calendar */
		command_status = execvp("./calendar_proc", (char*)0);
		if(command_status == -1) {
			fprintf(stderr, "Failed to exec calendar function\n");
			exit(1);
		}
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
	
  setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));	
	if(bind(listen_fd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {

		fprintf(stderr, "Failed to bind socket to port\n");
		exit(1);
	}

	freeaddrinfo(servinfo);

	if(listen(listen_fd, 10) == -1) {
		fprintf(stderr, "Failed to listen on port\n");
		exit(1);
	}

	while(1) {

		address_length = sizeof client_address;
		incoming_fd = accept(listen_fd, 
													(struct sockaddr *)&client_address,
													&address_length);

		if(incoming_fd == -1) {
			fprintf(stderr, "Failed to accept incoming connection/n");
		}	else {
			int pid = fork();
			if(pid == -1) {
				fprintf(stderr, "Error forking child process\n");
			} else if (pid == 0) {
				/* In child */
				/* Connect to calendar */
				int calendar_fd;
				memset(&hints, 0, sizeof hints);
				hints.ai_family = AF_UNSPEC;
				hints.ai_socktype = SOCK_STREAM;

				if((status = getaddrinfo("localhost", 
																	"40000", 
																	&hints, 
																	&servinfo)) != 0) {
					fprintf(stderr, "Failed to get calendar proc address\n");
				}

				calendar_fd = socket(servinfo->ai_family,
															servinfo->ai_socktype,
															servinfo->ai_protocol);

				connect(calendar_fd, servinfo->ai_addr, servinfo->ai_addrlen);
				
				recv(incoming_fd, (char*)&command, sizeof(CalendarCommand), 0);
				send(calendar_fd, (char*)&command, sizeof(CalendarCommand), 0);
				recv(calendar_fd, (char*)&response, sizeof(CalendarResponse), 0);
				if(response.response_code == GET) {
					while(response.response_code != GET_END) {
						send(incoming_fd, (char*)&response, sizeof(CalendarResponse), 0);
						recv(calendar_fd, (char*)&response, sizeof(CalendarResponse), 0);
					}
				}
				send(incoming_fd, (char*)&response, sizeof(CalendarResponse), 0);
				exit(0);
			}
		}
	}

	return 0;
}
