/* 
 * Iain Workman
 * ipw969
 * 11139430
 */
#include "network.h"

#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct connect_thread_args {
    int listen_fd;
    connection_set *connections;
} connection_thread_args;

void *listen_thread(void *thread_args) {
    int i_connection;
    connection_thread_args *args = (connection_thread_args *) thread_args;

    listen(args->listen_fd, 10);
    for (i_connection = 0;
         i_connection < args->connections->connection_count;
         i_connection++) {

        struct sockaddr_storage client_address;
        socklen_t address_size;
        int incoming_fd;

        address_size = sizeof client_address;
        incoming_fd = accept(args->listen_fd,
                             (struct sockaddr *) &client_address,
                             &address_size);

        args->connections->incoming_connections[i_connection] = incoming_fd;
    }

    return 0;
}

connection_set *init_connection_set(int connection_count) {

    connection_thread_args thread_args;
    pthread_t listen_thread_id;
    struct addrinfo hints, *server_results, *client_results;
    struct addrinfo *server_final, *client_final;

    int listen_fd;
    int return_code;
    int i_connection;
    int confirm = 1;

    if (connection_count <= 0) {
        return 0;
    }

    connection_set *new_set = malloc(sizeof(connection_set));

    if (new_set == 0) {
        return 0;
    }

    new_set->incoming_connections = malloc(sizeof(int) * connection_count);

    if (new_set->incoming_connections == 0) {
        free(new_set);
        return 0;
    }

    new_set->outgoing_connections = malloc(sizeof(int) * connection_count);

    if (new_set->outgoing_connections == 0) {
        free(new_set->incoming_connections);
        free(new_set);
        return 0;
    }

    new_set->connection_count = connection_count;
    thread_args.connections = new_set;

    /* Create server listen_fd*/
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((return_code = getaddrinfo("127.0.0.1", "30005", &hints,
                                   &server_results)) != 0) {
        free(new_set->incoming_connections);
        free(new_set->outgoing_connections);
        free(new_set);
        return 0;
    }

    for (server_final = server_results;
         server_final != NULL;
         server_final = server_final->ai_next) {

        if ((listen_fd = socket(server_final->ai_family,
                                server_final->ai_socktype,
                                server_final->ai_protocol)) == -1) {
            continue;
        }

        if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &confirm,
                       sizeof(int)) == -1) {
            free(new_set->incoming_connections);
            free(new_set->outgoing_connections);
            free(new_set);
            return 0;
        }

        if (bind(listen_fd,
                 server_final->ai_addr,
                 server_final->ai_addrlen) == -1) {
            close(listen_fd);
            continue;
        }

        break;
    }

    if (server_final == 0) {
        /* Could not bind to listen socket */
        free(new_set->incoming_connections);
        free(new_set->outgoing_connections);
        free(new_set);
        return 0;
    }

    thread_args.listen_fd = listen_fd;

    /* Spin up threads for listen/accept */
    return_code = pthread_create(&listen_thread_id,
                                 0,
                                 listen_thread,
                                 &thread_args);

    if (return_code == -1) {
        free(new_set->incoming_connections);
        free(new_set->outgoing_connections);
        free(new_set);
        return 0;
    }

    /* Make all the connections */
    for (i_connection = 0; i_connection < connection_count; i_connection++) {

        int connection_fd;

        if ((return_code = getaddrinfo("127.0.0.1", NULL, &hints,
                                       &client_results)) != 0) {
            free(new_set->incoming_connections);
            free(new_set->outgoing_connections);
            free(new_set);
            pthread_cancel(listen_thread_id);
            return 0;
        }

        for (client_final = client_results;
             client_final != NULL;
             client_final = client_final->ai_next) {


            if ((connection_fd = socket(client_final->ai_family,
                                        client_final->ai_socktype,
                                        client_final->ai_protocol)) == -1) {
                continue;
            }

            if (setsockopt(connection_fd, SOL_SOCKET, SO_REUSEADDR, &confirm,
                           sizeof(int)) == -1) {
                free(new_set->incoming_connections);
                free(new_set->outgoing_connections);
                free(new_set);
                pthread_cancel(listen_thread_id);
                return 0;
            }

            if (connect(connection_fd,
                        server_final->ai_addr,
                        server_final->ai_addrlen) == -1) {

                close(connection_fd);
                continue;
            }

            break;
        }

        if (client_final == 0) {
            /* Could not bind to socket */
            free(new_set->incoming_connections);
            free(new_set->outgoing_connections);
            free(new_set);
            pthread_cancel(listen_thread_id);
            return 0;
        }

        new_set->outgoing_connections[i_connection] = connection_fd;

        freeaddrinfo(client_results);
    }


    /* Join threads */
    pthread_join(listen_thread_id, NULL);

    freeaddrinfo(server_results);
    return new_set;
}

int next_incoming_connection(connection_set *connections) {

    int i_connection;
    int found_connection;
    if (connections == 0) {
        return -1;
    }

    for (i_connection = 0;
         i_connection < connections->connection_count; i_connection++) {
        if (connections->incoming_connections[i_connection] != 0) {
            found_connection = connections->incoming_connections[i_connection];
            connections->incoming_connections[i_connection] = 0;
            return found_connection;
        }

    }

    return -1;
}

int next_outgoing_connection(connection_set *connections) {


    int i_connection;
    int found_connection;
    if (connections == 0) {
        return -1;
    }

    for (i_connection = 0;
         i_connection < connections->connection_count; i_connection++) {
        if (connections->outgoing_connections[i_connection] != 0) {
            found_connection = connections->outgoing_connections[i_connection];
            connections->outgoing_connections[i_connection] = 0;
            return found_connection;
        }
    }

    return -1;
}