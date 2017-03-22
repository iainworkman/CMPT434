/* 
 * Iain Workman
 * ipw969
 * 11139430
 */
#ifndef CMPT434_DTN_NODE_H
#define CMPT434_DTN_NODE_H

#ifndef max
#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

#ifnef min
#define min( a, b ) ( ((a) < (b)) ? (a) : (b) )
#endif

#define MAX_POSITION 1000
#define MIN_POSITION 0

typedef struct dtn_node dtn_node;

/* A message to be carried by a DTN node */
typedef struct dtn_message {

    dtn_node* source;
    dtn_node* destination;
    int sequence_number;
} dtn_message;

/* A data tolerant network node */
typedef struct dtn_node {

    float x_position;
    float y_position;
    int next_sequence_number;
    dtn_message* message_buffer;
    int transmission_count;
    int next_buffer_slot;
}dtn_node;

/* Initializes a new dtn node with sensible initial values */
dtn_node* node_init(int buffer_size);

/* Frees the resources associated with a dtn node */
void node_free(dtn_node* node);

/* Checks whether two dtn nodes are in range
 * @returns: 1 if the two nodes are within the provided range, 0 otherwise */
int node_in_range(dtn_node* node1, dtn_node* node2, float range);

/* Moves the node a random amount in a provided direction, limited by
 * MAX_POS and MIN_POS
 * @returns 1 if the node was moved successfully, 0 otherwise */
int node_move(dtn_node* node, float distance);

/* Initializes a new message to be sent by a dtn node with the provided
 * sequence number */
dtn_message* message_init(int sequence_number);

/* Frees the resources associated with a dtn message */
void message_free(dtn_message* message);

#endif //CMPT434_DTN_NODE_H
