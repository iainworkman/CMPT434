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

int main(int argc, char** argv) {

	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo; 
	int socket_fd;

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

	send(socket_fd, "Test", sizeof(char) * 4, 0);

	freeaddrinfo(servinfo); // free the linked-list

	return 0;
}
