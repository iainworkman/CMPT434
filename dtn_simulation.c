/* 
 * Iain Workman
 * ipw969
 * 11139430
 */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "dtn.h"
#include "policies.h"


#define TIMESTEP_COUNT      100    /* K */
#define NODE_COUNT          15      /* N */
#define DESTINATION_COUNT   3       /* D */
#define NODE_BUFFER_SIZE    50      /* B */
#define BROADCAST_RANGE     50      /* R */
#define MOVE_DISTANCE       60      /* M */

typedef struct simulation_args {

    int timestep_count;
    int node_count;
    int destination_count;
    int node_buffer_size;
    int broadcast_range;
    int move_distance;
    int policy;

} simulation_args;

extern int verbose_mode;

void print_usage() {

    printf("\tdtn_simulation help: Print this message\n");
    printf("\tdtn_simulation [-v] <timesteps> <node_count> <destination_count> <node_buffer_size> <broadcast_range> <move_distance> <propagation_policy>\n");

    printf("\t-v (optional): Prints verbose output\n");
    printf("\ttimesteps: How many iterations of the simulation should be run\n");
    printf("\tnode_count: The number of nodes in the dtn grid\n");
    printf("\tdestination_count: The number of nodes which are considered a message destination\n");
    printf("\tnode_buffer_size: The amount of messages a node can hold\n");
    printf("\tbroadcast_range: How far a node can send a message\n");
    printf("\tmove_distance: How far each node will move in an iteration\n");
    printf("\tpropagation policy: The method used to determine if a node will broadcast a message\n");
    printf("\n\tThe following propagation policies are available:\n");
    printf("\t\tendemic: Will always attempt to send messages, messages will always be removed from sender\n");
    printf("\t\trandom: Will send with a 0.2 chance, messages will always be removed from sender\n");
}

int parse_args(int argc, char **argv, simulation_args *args) {

    if (!args) {
        return -1;
    }

    if (argc == 8) {

        verbose_mode = 0;
        args->timestep_count = strtol(argv[1], 0, 10);
        args->node_count = strtol(argv[2], 0, 10);
        args->destination_count = strtol(argv[3], 0, 10);
        args->node_buffer_size = strtol(argv[4], 0, 10);
        args->broadcast_range = strtol(argv[5], 0, 10);
        args->move_distance = strtol(argv[6], 0, 10);

        if (strcmp(argv[7], "endemic") == 0) {
            args->policy = ENDEMIC;
        } else if (strcmp(argv[7], "random") == 0) {
            args->policy = RANDOM;
        } else {
            return -1;
        }
    } else if (argc == 9) {
        if (strcmp(argv[1], "-v") == 0) {
            verbose_mode = 1;
        } else {
            return -1;
        }

        args->timestep_count = strtol(argv[2], 0, 10);
        args->node_count = strtol(argv[3], 0, 10);
        args->destination_count = strtol(argv[4], 0, 10);
        args->node_buffer_size = strtol(argv[5], 0, 10);
        args->broadcast_range = strtol(argv[6], 0, 10);
        args->move_distance = strtol(argv[7], 0, 10);

        if (strcmp(argv[8], "endemic") == 0) {
            args->policy = ENDEMIC;
        } else if (strcmp(argv[8], "random") == 0) {
            args->policy = RANDOM;
        } else {
            return -1;
        }
    } else {
        return -1;
    }

    return 0;
}

/* Input parameters:
 * move_distance ~ Distance each node should move in a given period
 * node_range ~ Distance each node can transmit */
int main(int argc, char** argv) {

    int return_code = 0;
    dtn_grid *grid = 0;
    simulation_settings settings;
    simulation_statistics statistics;
    simulation_args args;

    return_code = parse_args(argc, argv, &args);

    if (return_code != 0) {
        print_usage();
        return 1;
    }

    /* Init Grid */
    grid = init_grid(args.node_count,
                     args.destination_count,
                     args.node_buffer_size);

    srand(time(NULL));

    if (!grid) {
        fprintf(stderr, "[CRITICAL] Failed to init dtn grid\n");
        return 1;
    }

    settings.step_count = args.timestep_count;
    settings.broadcast_range = args.broadcast_range;
    settings.move_distance = args.move_distance;

    /* Run the simulation */
    if (args.policy == ENDEMIC) {
        return_code = run_simulation(grid, settings, endemic, &statistics);
    } else if (args.policy == RANDOM) {
        return_code = run_simulation(grid, settings, random_transmit,
                                     &statistics);
    }

    if (return_code == -1) {
        fprintf(stderr, "[ERROR] Could not run simulation\n");
        return 1;
    }

    printf("Results:\n");
    printf("\tFraction of messages which reached destination: %f\n",
           (float) statistics.messages_at_destination /
           (statistics.messages_at_destination +
            statistics.messages_in_flight +
            statistics.total_messages_lost));
    printf("\tBroadcasts required: %d\n", statistics.total_broadcasts);

    return 0;
}
