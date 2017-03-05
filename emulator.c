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
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>

#include "list.h"

#define MAX_BUFFER_SIZE 1024

/* Structs & Data Types */
typedef struct arguments {
    int drop_probability;
    int delay;
    int queue_length;
    char port_a[6];
    char port_b[6];
    char port_c[6];
} arguments;

typedef struct message {
    struct timeval time_received;
    char data[MAX_BUFFER_SIZE];
    int size;
} message;

/* Globals */

int port_a_fd;
struct addrinfo port_b_hints, *port_b_info;
struct addrinfo port_c_hints, *port_c_info;
LIST *port_b_queue;
sem_t port_b_queue_semaphore;
LIST *port_c_queue;
sem_t port_c_queue_semaphore;
arguments args;


/*
 * Helper function for parsing command line arguments
 */
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
    strcpy(result->port_b, argv[5]);
    strcpy(result->port_c, argv[6]);

    return 0;
}

/*
 * Helper function which prints out the required command line arguments
 */
void printUsage() {

    printf("\temulator <drop_probability> <delay> <queue_length> <port_a> <port_b> <port_c>\n\n");
    printf("\t<drop_probability> ~ The probability %% that a single packet will be lost (0-100)\n");
    printf("\t<delay> ~ The delay (ms) before a packet will be forwarded\n");
    printf("\t<queue_length> ~ The number of packets which can be queued on a given port\n");
    printf("\t<port_a>\n");
    printf("\t<port_b>\n");
    printf("\t<port_c>\n");
}

/*
 * Helper function which returns the time in ms between start and end
 */
long timedelta(struct timeval start, struct timeval end) {

    return (end.tv_sec - start.tv_sec) * 1000 +
           (end.tv_usec - start.tv_usec) / 1000;
}

/*
 * Thread which does the sending for port B
 */
void *portBSenderThread() {
    struct timespec sleep_time;
    long delta_time;
    struct timeval current_time;
    sleep_time.tv_nsec = (args.delay * 10000) / 2;

    while (1) {
        gettimeofday(&current_time, NULL);

        /* Ensure we have exclusive access to the message queue */
        sem_wait(&port_b_queue_semaphore);

        if (ListCount(port_b_queue) != 0) {
            /* We have messages in the queue */
            int still_to_send = 1;
            while (still_to_send) {
                /* Grab the next message in the queue */
                message *msg = ListFirst(port_b_queue);

                if (msg != NULL) {
                    /* Check how long its been in for */
                    delta_time = timedelta(msg->time_received, current_time);
                    if (delta_time > args.delay) {
                        /* Message has been in the queue for long enough */
                        ListRemove(port_b_queue);

                        /* Send the Message */
                        printf("Sending %s from B\n", msg->data);
                        sendto(port_a_fd, msg->data, msg->size, 0,
                               port_b_info->ai_addr, port_b_info->ai_addrlen);
                        free(msg);

                    } else {
                        /* First in queue hasn't waited the necessary delay time
                         * */
                        still_to_send = 0;
                    }

                } else {
                    /* No more in the queue */
                    still_to_send = 0;
                }
            }
        }
        sem_post(&port_b_queue_semaphore);
        /* Sleep for a bit so we're not hammering the queue */
        nanosleep(&sleep_time, 0);
    }

    return 0;
}

/*
 *  Thread which does the sending for port C
 */
void *portCSenderThread() {
    struct timespec sleep_time;
    long delta_time;
    struct timeval current_time;
    sleep_time.tv_nsec = (args.delay * 10000) / 2;

    while (1) {
        gettimeofday(&current_time, NULL);
        /* Ensure we have exclusive access to the message queue */
        sem_wait(&port_c_queue_semaphore);

        if (ListCount(port_c_queue) != 0) {
            /* We have messages in the queue */
            int still_to_send = 1;
            while (still_to_send) {
                /* Grab the next message in the queue*/
                message *msg = ListFirst(port_c_queue);

                if (msg != NULL) {
                    /* Check how long its been in for */
                    delta_time = timedelta(msg->time_received, current_time);
                    if (delta_time > args.delay) {
                        /* Message has been in the queue for long enough */
                        ListRemove(port_c_queue);

                        /* Send the Message */
                        sendto(port_a_fd, msg->data, msg->size, 0,
                               port_c_info->ai_addr, port_c_info->ai_addrlen);
                        free(msg);

                    } else {
                        /* First in queue hasn't waited the necessary delay time
                        * */
                        still_to_send = 0;
                    }


                } else {
                    /* No more in the queue */
                    still_to_send = 0;
                }
            }
        }
        sem_post(&port_c_queue_semaphore);
        nanosleep(&sleep_time, 0);
    }

    return 0;
}

/*
 * Function which adds a message entry to the provided queue which will have a
 * timer added associated with the provided timer_handler
 */
int enqueueMessage(LIST *queue,
                   sem_t *queue_semaphore,
                   char *data,
                   int data_length) {

    int return_code;

    message *msg = malloc(sizeof(message));
    if (msg == 0) {
        return -1;
    }

    /* Copy data across into the message struct */
    memcpy(msg->data, data, data_length);
    msg->size = data_length;

    /* Initialize message time */
    gettimeofday(&msg->time_received, NULL);

    /* Add to queue */
    sem_wait(queue_semaphore);
    return_code = ListAppend(queue, msg);
    if (return_code == -1) {
        free(msg);
    }
    sem_post(queue_semaphore);
    return return_code;
}


void *receiverThread() {
    int byte_count;
    char buffer[MAX_BUFFER_SIZE];
    struct sockaddr_in incoming_address;
    int keep_running = 1;
    socklen_t address_length;

    /* Start Listening */
    printf("Listening\n");
    address_length = sizeof incoming_address;

    while (keep_running) {
        byte_count = recvfrom(port_a_fd,
                              buffer,
                              MAX_BUFFER_SIZE - 1,
                              0,
                              (struct sockaddr *) &incoming_address,
                              &address_length);

        if (byte_count == -1) {
            continue;
        }

        if (ntohs(incoming_address.sin_port) == strtol(args.port_b, 0, 10)) {
            printf("port b (%d) sent:\n", ntohs(incoming_address.sin_port));
            printf("packet:\n");
            printf("\tlength: %d\n", byte_count);
            buffer[byte_count] = '\0';
            printf("\tcontent: %s\n", buffer);

            if ((rand() % 100) > args.drop_probability) {
                if (ListCount(port_c_queue) < args.queue_length) {
                    enqueueMessage(port_c_queue, &port_c_queue_semaphore,
                                   buffer, byte_count);
                } else {
                    printf("Port C queue full\n");
                }
            } else {
                printf("DROPPED!\n");
            }

        } else if (ntohs(incoming_address.sin_port) ==
                   strtol(args.port_c, 0, 10)) {
            printf("port c (%d) sent:\n", ntohs(incoming_address.sin_port));
            printf("packet:\n");
            printf("\tlength: %d\n", byte_count);
            buffer[byte_count] = '\0';
            printf("\tcontent: %s\n", buffer);

            if ((rand() % 100) > args.drop_probability) {
                if (ListCount(port_b_queue) < args.queue_length) {
                    enqueueMessage(port_b_queue,
                                   &port_b_queue_semaphore,
                                   buffer, byte_count);
                } else {
                    printf("Port B queue full\n");
                }
            } else {
                printf("DROPPED!\n");
            }

        } else {
            fprintf(stderr, "No idea who you are (%d).\n",
                    ntohs(incoming_address.sin_port));
        }
    }

    return 0;
}

int main(int argc, char **argv) {

    struct addrinfo port_a_hints, *port_a_info, *i_portinfo;
    int status_code;

    pthread_t senderb_thread, senderc_thread, receiver_thread;

    /* Set the random seed */
    srand(time(0));

    /* Parse Arguments */
    if (parseArguments(argc, argv, &args) == -1) {
        printUsage();
        return 1;
    }

    /* Build message queues */
    port_b_queue = ListCreate();
    port_c_queue = ListCreate();

    if (port_b_queue == 0 || port_c_queue == 0) {
        fprintf(stderr, "Could not create message queues\n");
        return 1;
    }

    /* Initialize queue semaphores */
    if (sem_init(&port_b_queue_semaphore, 0, 1) == -1) {
        fprintf(stderr, "Could not initialize port b queue semaphore\n");
        return 1;
    }

    if (sem_init(&port_c_queue_semaphore, 0, 1) == -1) {
        fprintf(stderr, "Could not initialize port c queue semaphore\n");
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

    memset(&port_b_hints, 0, sizeof port_b_hints);
    port_b_hints.ai_family = AF_UNSPEC;
    port_b_hints.ai_socktype = SOCK_DGRAM;
    port_b_hints.ai_flags = AI_PASSIVE;

    status_code = getaddrinfo(NULL, args.port_b, &port_b_hints, &port_b_info);
    if (status_code != 0) {
        fprintf(stderr, "Failed to get address info for port B\n");
        return 1;
    }

    memset(&port_c_hints, 0, sizeof port_c_hints);
    port_c_hints.ai_family = AF_UNSPEC;
    port_c_hints.ai_socktype = SOCK_DGRAM;
    port_c_hints.ai_flags = AI_PASSIVE;

    status_code = getaddrinfo(NULL, args.port_c, &port_c_hints, &port_c_info);
    if (status_code != 0) {
        fprintf(stderr, "Failed to get address info for port C\n");
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

    if (pthread_create(&receiver_thread, NULL, receiverThread, NULL) != 0) {

        fprintf(stderr, "Error creating receiver thread\n");
        return 1;
    }

    if (pthread_create(&senderb_thread, NULL, portBSenderThread, NULL) != 0) {

        fprintf(stderr, "Error creating port B sender thread\n");
        return 1;
    }

    if (pthread_create(&senderc_thread, NULL, portCSenderThread, NULL) != 0) {

        fprintf(stderr, "Error creating port C sender thread\n");
        return 1;
    }

    if (pthread_join(receiver_thread, NULL) != 0) {

        fprintf(stderr, "Error joining receiver thread\n");
        return 2;
    }

    if (pthread_join(senderb_thread, NULL) != 0) {

        fprintf(stderr, "Error joining port B sender thread\n");
        return 2;
    }

    if (pthread_join(senderc_thread, NULL) != 0) {

        fprintf(stderr, "Error joining port C sender thread\n");
        return 2;
    }

    return 0;
}
