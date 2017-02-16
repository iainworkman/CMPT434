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
#include <signal.h>
#include <time.h>

#include "list.h"

#define MAX_BUFFER_SIZE 1024

LIST *port_b_queue;
LIST *port_c_queue;
int port_a_fd;
struct addrinfo port_b_hints, *port_b_info;
struct addrinfo port_c_hints, *port_c_info;

typedef void(*timer_handler)(int sig, siginfo_t *si, void *uc);
typedef struct arguments {
    int drop_probability;
    int delay;
    int queue_length;
    char port_a[6];
    char port_b[6];
    char port_c[6];
} arguments;

typedef struct message {
    timer_t delay_timer;
    char data[MAX_BUFFER_SIZE];
    int size;
} message;

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
 * Handler for the timers in the port b message queue
 */
static void portBTimerHandler(int sig, siginfo_t *si, void *uc) {
    int bytes_sent = 0;
    printf("Sending to B\n");
    /* TODO: Disable timer interrupts*/
    ListFirst(port_b_queue);
    message *msg = ListRemove(port_b_queue);

    timer_delete(msg->delay_timer);
    bytes_sent = sendto(port_a_fd, msg->data, msg->size, 0,
                        port_b_info->ai_addr, port_b_info->ai_addrlen);

    free(msg);
    printf("%d bytes sent to b\n", bytes_sent);
    /* TODO: Re-enable timer interrupts */
}

/*
 * Handler for the timers in the port c message queue
 */
static void portCTimerHandler(int sig, siginfo_t *si, void *uc) {
    int bytes_sent = 0;
    printf("Sending to B\n");
    /* TODO: Disable timer interrupts*/
    ListFirst(port_b_queue);
    message *msg = ListRemove(port_c_queue);

    timer_delete(msg->delay_timer);
    bytes_sent = sendto(port_a_fd, msg->data, msg->size, 0,
                        port_c_info->ai_addr, port_c_info->ai_addrlen);

    free(msg);
    printf("%d bytes sent to c\n", bytes_sent);
    /* TODO: Re-enable timer interrupts */
}

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
 * Function which adds a message entry to the provided queue which will have a
 * timer added associated with the provided timer_handler
 */
int enqueueMessage(LIST *queue,
                   char *data,
                   int data_length,
                   int send_delay,
                   timer_handler handler) {

    int return_code;
    struct sigevent signal_event;
    struct itimerspec timer_specs;
    sigset_t signal_mask;
    struct sigaction signal_action;

    message *msg = malloc(sizeof(message));
    if (msg == 0) {
        return -1;
    }
    /* Copy data across into the message struct */
    memcpy(msg->data, data, data_length);
    msg->size = data_length;

    /* Initialize message timer */
    signal_action.sa_flags = SA_SIGINFO;
    signal_action.sa_sigaction = handler;
    sigemptyset(&signal_action.sa_mask);
    if (sigaction(SIGRTMIN, &signal_action, NULL) == -1) {
        free(msg);
        return -1;
    }

    signal_event.sigev_notify = SIGEV_SIGNAL;
    signal_event.sigev_signo = SIGRTMIN;
    signal_event.sigev_value.sival_ptr = &msg->delay_timer;
    if (timer_create(CLOCK_REALTIME, &signal_event, &msg->delay_timer) == -1) {
        free(msg);
        return -1;
    }

    timer_specs.it_value.tv_sec = send_delay / 1000;
    timer_specs.it_value.tv_nsec = send_delay * 1000;
    timer_specs.it_interval.tv_sec = timer_specs.it_value.tv_sec;
    timer_specs.it_interval.tv_nsec = timer_specs.it_value.tv_nsec;

    if (timer_settime(msg->delay_timer, 0, &timer_specs, NULL) == -1) {
        free(msg);
        return -1;
    }


    /* TODO: Shut off timer interrupts */
    return_code = ListAppend(queue, msg);
    if (return_code == -1) {
        free(msg);
    }
    /* TODO: Re-enable timer interrupts */
    return return_code;
}

int main(int argc, char **argv) {

    arguments args;
    struct addrinfo port_a_hints, *port_a_info, *i_portinfo;
    int status_code;
    int byte_count;
    struct sockaddr_in incoming_address;
    char buffer[MAX_BUFFER_SIZE];
    int keep_running = 1;
    socklen_t address_length;

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
                    enqueueMessage(port_c_queue, buffer, byte_count, args.delay,
                                   portCTimerHandler);
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
                    enqueueMessage(port_b_queue, buffer, byte_count, args.delay,
                                   portBTimerHandler);
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
