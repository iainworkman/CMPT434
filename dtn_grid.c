/* 
 * Iain Workman
 * ipw969
 * 11139430
 */
#include "dtn_grid.h"

#include "network.h"
#include <stdlib.h>
#include <unistd.h>

/* P'M'Vs */
dtn_link **links;

void *link_thread(void *link_thread_args) {

    int step_count;
    int i_current_step;
    dtn_node *self;
    dtn_node *other;
    dtn_grid *grid;

    for (i_current_step = 0; i_current_step < step_count; i_current_step++) {

        if (node_in_range(self, other && proagation_function(self, other))) {


        }
    }

}

/*
 * Creates communication links between the various nodes in the grid.
 * @returns: 0 if the grid was successfully linked, -1 otherwise
 */
int create_links(dtn_grid *grid) {

    int i_current_node, i_other_node;
    connection_set *connections;

    if (!grid) {
        return -1;
    }

    connections = init_connection_set(
            grid->node_count * (grid->node_count - 1));

    if (!connections) {
        return -1;
    }

    links = malloc(sizeof(dtn_link *) * grid->node_count);

    for (i_current_node = 0;
         i_current_node < grid->node_count; i_current_node++) {

        links[i_current_node] = malloc(sizeof(dtn_link) * grid->node_count);

        for (i_other_node = 0;
             i_other_node < grid->node_count; i_other_node++) {

            if (i_current_node == i_other_node) {

                continue;
            }

            links[i_current_node][i_other_node].checked_this_round = 0;
            links[i_current_node][i_other_node].incoming_socket =
                    next_incoming_connection(connections);
            links[i_current_node][i_other_node].outgoing_socket =
                    next_outgoing_connection(connections);
            links[i_current_node][i_other_node].self = &grid->nodes[i_current_node];
            links[i_current_node][i_other_node].other = &grid->nodes[i_other_node];

        }
    }

    return 0;
}

void free_links(dtn_grid *grid) {

    int i_current_node, i_other_node;

    for (i_current_node = 0;
         i_current_node < grid->node_count; i_current_node++) {

        for (i_other_node = 0;
             i_other_node < grid->node_count; i_other_node++) {

            if (i_other_node == i_current_node) {
                continue;
            }

            close(links[i_current_node][i_other_node].incoming_socket);
            close(links[i_current_node][i_other_node].outgoing_socket);
        }

        free(links[i_current_node]);
    }

    free(links);
}

/* Public Methods */

dtn_grid *grid_init(int node_count, int destination_count) {

    dtn_grid *new_grid = 0;

    if (node_count <= 0) {
        return 0;
    }

    if (destination_count <= 0) {
        return 0;
    }

    if (destination_count >= node_count) {
        return 0;
    }

    new_grid = malloc(sizeof(dtn_grid));

    if (!new_grid) {
        return 0;
    }

    new_grid->nodes = malloc(sizeof(dtn_node) * node_count);

    if (!new_grid->nodes) {
        free(new_grid);
        return 0;
    }

    new_grid->node_count = node_count;
    new_grid->destination_count = destination_count;

    return new_grid;
}

void grid_free(dtn_grid *grid) {

    if (!grid) {
        return;
    }

    free(grid->nodes);
    free(grid);
}

int grid_run(dtn_grid *grid,
             int step_count,
             int (*propagation_function)(dtn_node *, dtn_node *)) {


    if (!grid) {
        return -1;
    }

    if (step_count <= 0) {
        return -1;
    }

    if (!propagation_function) {
        return -1;
    }

    if (create_links(grid) == -1) {
        return -1;
    }

    /* Spin up the threads */
    /* for i = 0 to step_count -1: */

    free_links(grid);
    return 0;
}