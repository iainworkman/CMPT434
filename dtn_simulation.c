/* 
 * Iain Workman
 * ipw969
 * 11139430
 */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "dtn.h"


#define TIMESTEP_COUNT      100    /* K */
#define NODE_COUNT          15      /* N */
#define DESTINATION_COUNT   3       /* D */
#define NODE_BUFFER_SIZE    50      /* B */
#define BROADCAST_RANGE     50      /* R */
#define MOVE_DISTANCE       60      /* M */

int always_send(dtn_node *self,
                dtn_node *other,
                dtn_message *message,
                int *should_remove) {

    *should_remove = 1;
    return 1;
}

/* Input parameters:
 * move_distance ~ Distance each node should move in a given period
 * node_range ~ Distance each node can transmit */
int main(int argc, char** argv) {

    int return_code = 0;
    dtn_grid *grid = 0;
    simulation_settings settings;
    simulation_statistics statistics;

    /* Init Grid */
    grid = init_grid(NODE_COUNT, DESTINATION_COUNT, NODE_BUFFER_SIZE);

    srand(time(NULL));

    if (!grid) {
        fprintf(stderr, "[CRITICAL] Failed to init dtn grid\n");
        return 1;
    }

    settings.step_count = TIMESTEP_COUNT;
    settings.broadcast_range = BROADCAST_RANGE;
    settings.move_distance = MOVE_DISTANCE;

    /* Run the simulation */
    return_code = run_simulation(grid, settings, always_send, &statistics);

    if (return_code == -1) {
        fprintf(stderr, "[ERROR] Could not run simulation\n");
        return 1;
    }

    printf("Simulation Results:\n");
    printf("Messages at destination: %d\n", statistics.messages_at_destination);
    printf("Broadcasts required: %d\n", statistics.total_broadcasts);
    printf("Messages in flight: %d\n", statistics.messages_in_flight);
    printf("Messages lost: %d\n", statistics.total_messages_lost);

    /* Repeat K Times: */
    /* Generate data packet at each node, add it to the buffer */
    /* For each node: */
    /* Move move_distance in random direction */
    /* Check if in range of all other nodes */
    /* Transmit based on policy and node_range */
    /* end For each node */
    /* end repeat K times */

}
