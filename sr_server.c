/*
 * Iain Workman
 * ipw969
 * 11139430
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 64
#define WINDOW_SIZE     5
#define SEQUENCE_SIZE   (WINDOW_SIZE*2)

typedef struct arguments {
    char listen_port[6];    /* Server's listen port */
    char ack_port[6];       /* The client's ack listen port */
} arguments;

void printUsage() {

    printf("\tsr_server <listen_port> <ack_port>\n\n");
    printf("\t<listen_port> ~ The port on which this server will listen\n");
    printf("\t<ack_port> ~ The port to send ACKs to\n");
}

/*
 * Helper function for parsing command line arguments
 */
int parseArguments(int argc, char **argv, arguments *result) {

    int listen_port = 0;
    int ack_port = 0;

    if (argc != 3) {
        return -1;
    }

    listen_port = strtol(argv[1], 0, 10);
    if (listen_port < 30000 || listen_port > 40000) {
        return -1;
    }

    ack_port = strtol(argv[2], 0, 10);
    if (ack_port < 30000 || ack_port > 40000 || listen_port == ack_port) {
        return -1;
    }

    strcpy(result->listen_port, argv[1]);
    strcpy(result->ack_port, argv[2]);
    return 0;
}

/*
 * Checks whether the provided sequence number is within the window
 */
int is_in_window(int sequence_number, int window_start, int window_end) {
    if (window_start < window_end) {
        if (sequence_number >= window_start &&
            sequence_number <= window_end) {
            return 1;
        }
    } else {
        if (sequence_number >= window_start ||
            sequence_number <= window_end) {
            return 1;
        }
    }

    return 0;
}

/*
 * Based on DGRAM example in Beej's Network Guide
 * http://beej.us/guide/bgnet/output/html/multipage/clientserver.html#datagram
 */
int main(int argc, char **argv) {

    int listen_fd;
    int window_start;
    int window_end;
    int accepted[SEQUENCE_SIZE];
    struct sockaddr_storage client_address;
    socklen_t address_length;
    struct addrinfo *ack_info;
    struct addrinfo listen_hints, *listen_info, *i_addressinfo;
    struct addrinfo ack_hints;
    int status_code;
    arguments args;
    int received_sequence_number;
    int converted_sequence_number;
    int i_window_iterator;
    int out_of_sequence = 0;
    int bytes_received;

    /* Init globals */
    window_start = 0;
    window_end = WINDOW_SIZE - 1;
    memset(accepted, 0, sizeof(int) * SEQUENCE_SIZE);

    if (parseArguments(argc, argv, &args) == -1) {
        printUsage();
        return 1;
    }

    memset(&listen_hints, 0, sizeof listen_hints);
    listen_hints.ai_family = AF_INET;
    listen_hints.ai_socktype = SOCK_DGRAM;
    listen_hints.ai_flags = AI_PASSIVE;

    if ((status_code = getaddrinfo(NULL,
                                   args.listen_port,
                                   &listen_hints,
                                   &listen_info)) != 0) {

        fprintf(stderr,
                "Failed to get address info: %s\n",
                gai_strerror(status_code));
        return 1;
    }

    /* loop through all the results and bind to the first we can */
    for (i_addressinfo = listen_info;
         i_addressinfo != NULL;
         i_addressinfo = i_addressinfo->ai_next) {

        if ((listen_fd = socket(i_addressinfo->ai_family,
                                i_addressinfo->ai_socktype,
                                i_addressinfo->ai_protocol)) == -1) {

            continue;
        }

        status_code = bind(listen_fd,
                           i_addressinfo->ai_addr,
                           i_addressinfo->ai_addrlen);

        if (status_code == -1) {
            close(listen_fd);
            continue;
        }

        break;
    }

    if (i_addressinfo == NULL) {
        fprintf(stderr, "Could not bind to listen socket\n");
        return 1;
    }

    freeaddrinfo(listen_info);

    memset(&ack_hints, 0, sizeof ack_hints);
    ack_hints.ai_family = AF_INET;
    ack_hints.ai_socktype = SOCK_DGRAM;

    if ((status_code = getaddrinfo(NULL,
                                   args.ack_port,
                                   &ack_hints,
                                   &ack_info)) != 0) {

        fprintf(stderr,
                "Failed to get address info: %s\n",
                gai_strerror(status_code));
        return 1;
    }

    while (1) {
        address_length = sizeof client_address;

        /* receive message */
        bytes_received = recvfrom(listen_fd,
                                  &received_sequence_number,
                                  sizeof(int),
                                  0,
                                  (struct sockaddr *) &client_address,
                                  &address_length);

        if (bytes_received == -1) {
            continue;
        }

        received_sequence_number = ntohl(received_sequence_number);
        if (is_in_window(received_sequence_number, window_start, window_end) ==
            0) {
            fprintf(stderr,
                    "Rejected message (%d) as outside of window\n",
                    received_sequence_number);
        }

        /* Mark as arrived */
        accepted[received_sequence_number] = 1;

        converted_sequence_number = htonl(received_sequence_number);
        /* Send the ACK */
        sendto(listen_fd,
               &converted_sequence_number,
               sizeof(int),
               0,
               ack_info->ai_addr,
               ack_info->ai_addrlen
        );

        /* Send to network layer all items which are arrived and in order */

        for (i_window_iterator = 0;
             i_window_iterator < WINDOW_SIZE && out_of_sequence == 0;
             ++i_window_iterator) {

            int i_window_position =
                    (i_window_iterator + window_start) % SEQUENCE_SIZE;

            if (accepted[i_window_position] == 1) {
                /* Reset the accepted status so it can be used next time around */
                accepted[i_window_position] = 0;
                /* 'Send' to the network layer */
                printf("Sending %d to the network layer\n", i_window_position);

                /* Advance the Window */
                window_start = (window_start + 1) % SEQUENCE_SIZE;
                window_end = (window_end + 1) % SEQUENCE_SIZE;

            } else {
                out_of_sequence = 0;
            }
        }
    }

    close(listen_fd);
    return 0;
}
