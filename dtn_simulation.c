/* 
 * Iain Workman
 * ipw969
 * 11139430
 */
#include <stdio.h>

#include "dtn_grid.h"
#include "dtn_node.h"


#define TIMESTEP_COUNT      1000    /* K */
#define NODE_COUNT          15      /* N */
#define DESTINATION_COUNT   3       /* D */
#define NODE_BUFFER_SIZE    10      /* B */

int always_send(dtn_node *self, dtn_node *other) {

    return 1;
}

/* Input parameters:
 * move_distance ~ Distance each node should move in a given period
 * node_range ~ Distance each node can transmit */
int main(int argc, char** argv) {

    int i_node = 0;
    int return_code = 0;
    dtn_grid *grid = 0;

    /* Init Grid */
    grid = grid_init(NODE_COUNT, DESTINATION_COUNT);

    if (!grid) {
        fprintf(stderr, "[CRITICAL] Failed to init dtn grid\n");
        return 1;
    }

    /* Init N nodes */
    for (i_node = 0; i_node < NODE_COUNT; i_node++) {

        return_code = node_init(&grid->nodes[i_node], NODE_BUFFER_SIZE, i_node);
        if (return_code == -1) {
            fprintf(stderr, "[CRITICAL] Failed to init dtn node %d\n", i_node);
            return 1;
        }
    }

    /* Run the simulation */
    return_code = grid_run(grid, TIMESTEP_COUNT, always_send);
    if (return_code == -1) {
        fprintf(stderr, "[ERROR] Could not run simulation\n");
        return 1;
    }


    /* Repeat K Times: */
    /* Generate data packet at each node, add it to the buffer */
    /* For each node: */
    /* Move move_distance in random direction */
    /* Check if in range of all other nodes */
    /* Transmit based on policy and node_range */
    /* end For each node */
    /* end repeat K times */

}
