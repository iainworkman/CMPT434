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
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <semaphore.h>

#define MESSAGE_COUNT   100
#define WINDOW_SIZE     5
#define SEQUENCE_SIZE   (WINDOW_SIZE*2)
#define TIMEOUT_MS      4000

/* Globals for the server */
int send_fd;
struct addrinfo *server_info;
sem_t free_to_send;
sem_t sending;
socklen_t address_length;
int next_sequence_number;
int window_start;
int window_end;
int accepted[SEQUENCE_SIZE];
struct timeval time_sent[SEQUENCE_SIZE];
struct sockaddr_storage server_address;

/* Globals for statistics */
int items_sent = 0;
int acks_received = 0;
int retransmissions = 0;

typedef struct arguments {
    char server_port[6];    /* Server's listen port */
    char ack_port[6];       /* Client's ack listen port */
} arguments;

void printUsage() {

    printf("\tsr_client <server_port> <ack_port>\n\n");
    printf("\t<server_port> ~ The port to which the client will send\n");
    printf("\t<ack_port> ~ The port to receive ACKs on\n");
}

/*
 * Helper function for parsing command line arguments
 */
int parseArguments(int argc, char **argv, arguments *result) {

    int server_port = 0;
    int ack_port = 0;

    if (argc != 3) {
        return -1;
    }

    server_port = strtol(argv[1], 0, 10);
    if (server_port < 30000 || server_port > 40000) {
        return -1;
    }

    ack_port = strtol(argv[2], 0, 10);
    if (ack_port < 30000 || ack_port > 40000 || server_port == ack_port) {
        return -1;
    }

    strcpy(result->server_port, argv[1]);
    strcpy(result->ack_port, argv[2]);
    return 0;
}

/*
 * Checks whether the provided sequence number is within the window
 */
int is_in_window(int sequence_number) {
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

void *receiverThread() {

    int sent_sequence_number;
    int i_window_iterator;
    int out_of_sequence = 0;
    int status_code;
    int byte_count;
    int amount_to_increment_window;
    address_length = sizeof server_address;

    while (acks_received < MESSAGE_COUNT) {
        /* receive ack response */
        byte_count = recvfrom(send_fd,
                              &sent_sequence_number,
                              sizeof(int),
                              0,
                              (struct sockaddr *) &server_address,
                              &address_length);

        if (byte_count == -1) {
            continue;
        }


        sent_sequence_number = ntohl(sent_sequence_number);
        if (is_in_window(sent_sequence_number) == 0) {
            fprintf(stderr,
                    "\tReceieved ACK outside of window (%d [%d - %d])\n",
                    sent_sequence_number, window_start, window_end);
            continue;
        }

        printf("\tReceived ACK (%d)", sent_sequence_number);
        acks_received++;
        /* Mark as arrived */
        accepted[sent_sequence_number] = 1;

        /* Process changes to the window */
        amount_to_increment_window = 0;
        out_of_sequence = 0;
        for (i_window_iterator = 0;
             i_window_iterator < WINDOW_SIZE && out_of_sequence == 0;
             ++i_window_iterator) {

            int i_window_position =
                    (i_window_iterator + window_start) % SEQUENCE_SIZE;

            if (accepted[i_window_position] == 1) {
                /* Reset the accepted status so it can be used next time around */
                accepted[i_window_position] = 0;
                /* Advance the Window */
                amount_to_increment_window++;


            } else {
                out_of_sequence = 1;
            }
        }
        if (amount_to_increment_window > 0) {
            window_start =
                    (window_start + amount_to_increment_window) % SEQUENCE_SIZE;
            window_end =
                    (window_end + amount_to_increment_window) % SEQUENCE_SIZE;
            printf("Window is now [%d - %d]\n", window_start, window_end);
        } else {
            printf("\n");
        }

        while (amount_to_increment_window > 0) {
            /* Wake up the sender (potentially) */
            status_code = sem_post(&free_to_send);

            if (status_code == -1) {
                fprintf(stderr, "Could not P the semaphore\n");
            }

            amount_to_increment_window--;
        }
    }
    return 0;
}

void *senderThread() {

    int i_message;
    int status_code;
    int converted_number;

    for (i_message = 0; i_message < MESSAGE_COUNT; i_message++) {
        status_code = sem_wait(&free_to_send);

        if (status_code == -1) {
            fprintf(stderr, "Could not V the window semaphore\n");
            return 0;
        }

        status_code = sem_wait(&sending);

        if (status_code == -1) {
            fprintf(stderr, "Could not V the sending semaphore\n");
            return 0;
        }

        /* Send the next message from the window */
        printf("Sending: %d\n", next_sequence_number);
        converted_number = htonl(next_sequence_number);
        sendto(
                send_fd,
                &converted_number,
                sizeof(int),
                0,
                server_info->ai_addr,
                server_info->ai_addrlen
        );

        /* Setup timout timer */
        gettimeofday(&time_sent[next_sequence_number], NULL);

        next_sequence_number = (next_sequence_number + 1) % SEQUENCE_SIZE;
        items_sent++;

        status_code = sem_post(&sending);

        if (status_code == -1) {
            fprintf(stderr, "Could not P the sending semaphore\n");
            return 0;
        }
    }
    return 0;
}

long timedelta(struct timeval start, struct timeval end) {

    return (end.tv_sec - start.tv_sec) * 1000 +
           (end.tv_usec - start.tv_usec) / 1000;
}

void *timeoutThread() {
    int i_window_iterator;
    int status_code;
    struct timespec sleep_time;
    long delta_time;

    sleep_time.tv_nsec = (TIMEOUT_MS * 10000) / 2;

    while (acks_received < MESSAGE_COUNT) {
        for (i_window_iterator = 0;
             i_window_iterator < WINDOW_SIZE; i_window_iterator++) {

            int window_index =
                    (i_window_iterator + window_start) % SEQUENCE_SIZE;
            struct timeval current_time;
            int converted_number;

            if (accepted[window_index] == 1) {
                continue;
            }

            gettimeofday(&current_time, NULL);

            delta_time = timedelta(time_sent[window_index], current_time);
            if (delta_time > TIMEOUT_MS) {

                status_code = sem_wait(&sending);

                if (status_code == -1) {
                    fprintf(stderr, "Could not V the sending semaphore\n");
                    return 0;
                }

                /* Send the next message from the window */
                printf("\t\tRe-Sending: %d\n", window_index);
                converted_number = htonl(window_index);
                sendto(
                        send_fd,
                        &converted_number,
                        sizeof(int),
                        0,
                        server_info->ai_addr,
                        server_info->ai_addrlen
                );

                /* Resetup timout timer */
                gettimeofday(&time_sent[window_index], NULL);
                retransmissions++;

                status_code = sem_post(&sending);

                if (status_code == -1) {
                    fprintf(stderr, "Could not P the sending semaphore\n");
                    return 0;
                }
            }

            nanosleep(&sleep_time, 0);
        }
    }

    return 0;
}

/*
 * Based on DGRAM example in Beej's Network Guide
 * http://beej.us/guide/bgnet/output/html/multipage/clientserver.html#datagram
 */
int main(int argc, char **argv) {

    struct addrinfo server_hints, *i_addressinfo;
    struct addrinfo ack_hints, *ack_info;
    int status_code;
    arguments args;
    pthread_t sender_thread, receiver_thread, timeout_thread;

    /* Init globals */
    window_start = 0;
    window_end = WINDOW_SIZE - 1;
    next_sequence_number = 0;
    memset(accepted, 0, sizeof(int) * SEQUENCE_SIZE);

    /* Parse command line arguments */

    if (parseArguments(argc, argv, &args) == -1) {
        printUsage();
        return 1;
    }

    memset(&ack_hints, 0, sizeof ack_hints);
    ack_hints.ai_family = AF_INET;
    ack_hints.ai_socktype = SOCK_DGRAM;
    ack_hints.ai_flags = AI_PASSIVE;

    /* Create message queue semaphore */
    status_code = sem_init(&free_to_send, 0, WINDOW_SIZE);
    if (status_code == -1) {
        fprintf(stderr, "Failed to initialize window semaphore\n");
        return 1;
    }

    status_code = sem_init(&sending, 0, 1);
    if (status_code == -1) {
        fprintf(stderr, "Failed to initialize sender semaphore\n");
        return -1;
    }


    /* Get the required address info */
    if ((status_code = getaddrinfo(NULL,
                                   args.ack_port,
                                   &ack_hints,
                                   &ack_info)) != 0) {

        fprintf(stderr,
                "Failed to get address info: %s\n",
                gai_strerror(status_code));
        return 1;
    }

    for (i_addressinfo = ack_info;
         i_addressinfo != NULL;
         i_addressinfo = i_addressinfo->ai_next) {

        if ((send_fd = socket(i_addressinfo->ai_family,
                              i_addressinfo->ai_socktype,
                              i_addressinfo->ai_protocol)) == -1) {

            continue;
        }

        status_code = bind(send_fd,
                           i_addressinfo->ai_addr,
                           i_addressinfo->ai_addrlen);

        if (status_code == -1) {
            close(send_fd);
            continue;
        }

        break;
    }

    if (i_addressinfo == NULL) {
        fprintf(stderr, "Could not bind to listen socket\n");
        return 1;
    }

    freeaddrinfo(ack_info);

    memset(&server_hints, 0, sizeof server_hints);
    server_hints.ai_family = AF_INET;
    server_hints.ai_socktype = SOCK_DGRAM;

    if ((status_code = getaddrinfo(NULL,
                                   args.server_port,
                                   &server_hints,
                                   &server_info)) != 0) {

        fprintf(stderr,
                "Failed to get address info: %s\n",
                gai_strerror(status_code));
        return 1;
    }

    if (pthread_create(&sender_thread, NULL, senderThread, 0) != 0) {

        fprintf(stderr, "Error creating sender thread\n");
        return 1;
    }

    if (pthread_create(&receiver_thread, NULL, receiverThread, NULL) != 0) {

        fprintf(stderr, "Error creating receiver thread\n");
        return 1;
    }

    if (pthread_create(&timeout_thread, NULL, timeoutThread, NULL) != 0) {

        fprintf(stderr, "Error creating timeout thread'n");
        return 1;
    }

    if (pthread_join(sender_thread, NULL) != 0) {

        fprintf(stderr, "Error joining sender thread\n");
        return 2;

    }

    if (pthread_join(receiver_thread, NULL) != 0) {

        fprintf(stderr, "Error joining receiver thread\n");
        return 2;

    }

    if (pthread_join(timeout_thread, NULL) != 0) {

        fprintf(stderr, "Error joining timeout thread\n");
        return 2;
    }

    printf("Messages Sent: %d\n", items_sent);
    printf("ACKs Received: %d\n", acks_received);
    printf("Messages Resent: %d\n", retransmissions);

    close(send_fd);
    return 0;
}
