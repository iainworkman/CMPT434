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
	fd_set master_fds;
	fd_set read_fds;
	int max_fd;
	int i_fd;
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

	if(argc == 1) {
		fprintf(stderr, "No listen port provided\n");
		exit(1);
	}
	
	port = strtol(argv[1], &parse_test, 0);
	if(port < 30001 || port > 40000 || *parse_test != '\0') {
		fprintf(stderr, "Port is not a valid number (30001 - 40000)\n");
		exit(1);
	}	
	
	if(CalendarInit() != 0) {
		fprintf(stderr, "Failed to initialize Calendar subsystem\n");
		exit(1);
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
					}
				} else {
					bytes_read = recv(i_fd, (char*)&command, sizeof(CalendarCommand), 0);
					if (bytes_read == 0) {
						close(i_fd);
						FD_CLR(i_fd, &master_fds);
					} else if (bytes_read < 0) {
						fprintf(stderr, "Failed receiving data\n");
						close(i_fd);
						FD_CLR(i_fd, &master_fds);
					} else {
						if(command.command_code == ADD_EVENT) {
							temp_entry = (CalendarEntry*)malloc(sizeof(CalendarEntry));
							*temp_entry = command.event;
							command_status = CalendarAdd(temp_entry, command.username);
							if(command_status != 0) {
								response.response_code = command_status;	
							} else {
								response.response_code = ADD_SUCCESS;
							}
							send(i_fd, (char*)&response, sizeof(CalendarResponse),0);
						} else if (command.command_code == REMOVE_EVENT) {
							command_status = CalendarRemove(&command.event, command.username);
							if(command_status != 0) {
								response.response_code = command_status;
							} else {
								response.response_code = REMOVE_SUCCESS;
							}
							send(i_fd, (char*)&response, sizeof(CalendarResponse), 0);
						} else if (command.command_code == UPDATE_EVENT) {
							CalendarEntry locate_entry;
							locate_entry.date = command.event.date;
							locate_entry.start_time = command.event.start_time;
							locate_entry.end_time.empty = 1;
							command_status = 
								CalendarUpdate(&locate_entry, &command.event, command.username);
							
							if(command_status != 0) {
								response.response_code = command_status;
							} else {
								response.response_code = UPDATE_SUCCESS;
							}
							send(i_fd, (char*)&response, sizeof(CalendarResponse), 0);
						} else if (command.command_code == GET_EVENTS) {
							CalendarEntry* current_entry = 0;
							get_returns = CalendarGetEntries(&command.event, command.username);
							current_entry = ListFirst(get_returns->entries);
							while(current_entry) {
								response.response_code = GET;
								response.entry = *current_entry;
								send(i_fd, (char*)&response, sizeof(CalendarResponse), 0);
								current_entry = ListNext(get_returns->entries);
							}
							response.response_code = GET_END;
							send(i_fd, (char*)&response, sizeof(CalendarResponse), 0);	
						}	else {

						}					
					}
				}
			}
		}
	}

	return 0;
}
