/* 
 * Iain Workman
 * ipw969
 * 11139430
 */
#ifndef CMPT434_NETWORK_H
#define CMPT434_NETWORK_H

typedef struct connection_set {
    int *incoming_connections;
    int *outgoing_connections;
    int connection_count;
} connection_set;

connection_set *init_connection_set(int connection_count);

int next_incoming_connection(connection_set *connections);

int next_outgoing_connection(connection_set *connections);

#endif //CMPT434_NETWORK_H
