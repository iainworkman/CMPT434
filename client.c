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
	CalendarEntry entry;

	/* Parse Arguments */
	if(argc < 5) {
		fprintf(stderr, "Insufficient arguments provided\n");
		exit(1);
	}

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
	
	entry.date.year = 16;
	entry.date.month = 12;
	entry.date.day = 10;
	entry.date.empty = 0;

	entry.start_time.hour = 14;
	entry.start_time.minute = 10;
	entry.start_time.empty = 0;

	entry.end_time.empty = 1;

	strcpy(entry.name, "Test Sending an Entry");
	
	send(socket_fd, (char*)&entry, sizeof(CalendarEntry), 0);

	freeaddrinfo(servinfo); // free the linked-list

	return 0;
}
