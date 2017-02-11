/* 
 * Iain Workman
 * ipw969
 * 11139430
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAX_BUFFER_SIZE 1024

typedef struct arguments {
    int drop_probability;
    int delay;
    int queue_length;
    char port_a[6];
    int port_b;
    int port_c;
} arguments;

void printUsage() {

    printf("\temulator <drop_probability> <delay> <queue_length> <port_a> <port_b> <port_c>\n\n");
    printf("\t<drop_probability> ~ The probability %% that a single packet will be lost (0-100)\n");
    printf("\t<delay> ~ The delay (ms) before a packet will be forwarded\n");
    printf("\t<queue_length> ~ The number of packets which can be queued on a given port\n");
    printf("\t<port_a>\n");
    printf("\t<port_b>\n");
    printf("\t<port_c>\n");
}

int parseArguments(int argc, char **argv, arguments *result) {

    int drop_probability = 0;
    int delay = 0;
    int queue_length = 0;
    int port_a = 0;
    int port_b = 0;
    int port_c = 0;

    if (argc != 7) {
        return -1;
    }

    drop_probability = strtol(argv[1], 0, 10);
    delay = strtol(argv[2], 0, 10);
    queue_length = strtol(argv[3], 0, 10);
    port_a = strtol(argv[4], 0, 10);
    port_b = strtol(argv[5], 0, 10);
    port_c = strtol(argv[6], 0, 10);

    if (drop_probability < 0 || drop_probability > 100) {
        return -1;
    }

    if (delay < 0) {
        return -1;
    }

    if (queue_length <= 0) {
        return -1;
    }

    if (port_a < 30000 || port_a > 40000) {
        return -1;
    }

    if (port_b < 30000 || port_b > 40000) {
        return -1;
    }

    if (port_c < 30000 || port_c > 40000) {

        return -1;
    }

    if (port_a == port_b || port_a == port_c || port_b == port_c) {

        return -1;
    }

    result->drop_probability = drop_probability;
    result->delay = delay;
    result->queue_length = queue_length;
    strcpy(result->port_a, argv[4]);
    result->port_b = port_b;
    result->port_c = port_c;

    return 0;
}

int main(int argc, char **argv) {

    arguments args;
    int port_a_fd;
    struct addrinfo port_a_hints, *port_a_info, *i_portinfo;
    int status_code;
    int byte_count;
    struct sockaddr_in incoming_address;
    char buffer[MAX_BUFFER_SIZE];
    int keep_running = 1;
    socklen_t address_length;

    /* Parse Arguments */
    if (parseArguments(argc, argv, &args) == -1) {
        printUsage();
        return 1;
    }

    /* Get address info for A, B and C */
    memset(&port_a_hints, 0, sizeof port_a_hints);
    port_a_hints.ai_family = AF_UNSPEC;
    port_a_hints.ai_socktype = SOCK_DGRAM;
    port_a_hints.ai_flags = AI_PASSIVE;

    status_code = getaddrinfo(NULL, args.port_a, &port_a_hints, &port_a_info);
    if (status_code != 0) {
        fprintf(stderr, "Failed to get address info for port A\n");
        return 1;
    }

    /* bind to A */

    for (i_portinfo = port_a_info;
         i_portinfo != NULL;
         i_portinfo = i_portinfo->ai_next) {

        if ((port_a_fd = socket(i_portinfo->ai_family, i_portinfo->ai_socktype,
                                i_portinfo->ai_protocol)) == -1) {
            continue;
        }

        if (bind(port_a_fd, i_portinfo->ai_addr, i_portinfo->ai_addrlen) ==
            -1) {
            close(port_a_fd);
            continue;
        }

        break;
    }

    if (i_portinfo == NULL) {

        fprintf(stderr, "Failed to bind port A to socket\n");
        return 1;
    }

    /* Start Listening */
    printf("Listening\n");
    while (keep_running) {
        address_length = sizeof incoming_address;
        byte_count = recvfrom(port_a_fd,
                              buffer,
                              MAX_BUFFER_SIZE - 1,
                              0,
                              (struct sockaddr *) &incoming_address,
                              &address_length);

        if (byte_count == -1) {
            fprintf(stderr, "Failed during receive\n");
            return 1;
        }

        if (ntohs(incoming_address.sin_port) == args.port_c) {
            printf("port c (%d) sent:\n", ntohs(incoming_address.sin_port));
            printf("packet:\n");
            printf("\tlength: %d\n", byte_count);
            buffer[byte_count] = '\0';
            printf("\tcontent: %s\n", buffer);

        } else if (ntohs(incoming_address.sin_port) == args.port_b) {
            printf("port b (%d) sent:\n", ntohs(incoming_address.sin_port));
            printf("packet:\n");
            printf("\tlength: %d\n", byte_count);
            buffer[byte_count] = '\0';
            printf("\tcontent: %s\n", buffer);

        } else {
            fprintf(stderr, "No idea who you are (%d).\n",
                    ntohs(incoming_address.sin_port));
        }
    }

    return 0;
}
