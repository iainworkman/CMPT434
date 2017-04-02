/* 
 * Iain Workman
 * ipw969
 * 11139430
 */
#ifndef CMPT434_DTN_H
#define CMPT434_DTN_H

#include "list.h"

#ifndef max
#define max(a, b) ( ((a) > (b)) ? (a) : (b) )
#endif

#ifndef min
#define min(a, b) ( ((a) < (b)) ? (a) : (b) )
#endif

#define MAX_POSITION 1000
#define MIN_POSITION 0

int verbose_mode;

typedef struct dtn_node {
    int id;
    float x_position;
    float y_position;
    LIST *messages;
    int buffer_size;
    int broadcasts_made;
    int messages_lost;

} dtn_node;

typedef struct dtn_grid {
    dtn_node *nodes;
    int node_count;
    int destination_count;
} dtn_grid;

typedef struct dtn_message {
    int id;
    dtn_node *destination;
    dtn_node *source;
    int hops_taken;
} dtn_message;

typedef struct simulation_settings {
    float broadcast_range;
    float move_distance;
    int step_count;
} simulation_settings;

typedef struct simulation_statistics {
    int messages_at_destination;
    int total_broadcasts;
    int messages_in_flight;
    int total_messages_lost;
} simulation_statistics;

dtn_grid *
init_grid(int node_count, int destination_count, int node_buffer_size);

int run_simulation(dtn_grid *grid,
                   simulation_settings settings,
                   int (*propagation_function)(dtn_node *, dtn_node *,
                                               dtn_message *, int *),
                   simulation_statistics *statistics);

/* Checks whether two dtn nodes are in range
 * @returns: 1 if the two nodes are within the provided range, 0 otherwise */
int node_in_range(dtn_node *node1, dtn_node *node2, float range);

/* Moves the node a random amount in a provided direction, limited by
 * MAX_POS and MIN_POS
 * @returns 1 if the node was moved successfully, 0 otherwise */
int node_move(dtn_node *node, float distance);

#endif
