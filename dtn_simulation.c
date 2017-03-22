/* 
 * Iain Workman
 * ipw969
 * 11139430
 */
#include <stdio.h>

#define TIMESTEP_COUNT      1000    /* K */
#define NODE_COUNT          15      /* N */
#define DESTINATION_COUNT   3       /* D */

/* Input parameters:
 * move_distance ~ Distance each node should move in a given period
 * node_range ~ Distance each node can transmit */
int main(int argc, char** argv) {

    /* Init N nodes */
    /* Make first D nodes destinations */
    /* Spin up threads for each node pair */

    /* Repeat K Times: */
    /* Generate data packet at each node, add it to the buffer */
    /* For each node:
    /* Move move_distance in random direction */
    /* Check if in range of all other nodes */
    /* Transmit based on policy and node_range */
    /* end For each node */
    /* end repeat K times */

}
