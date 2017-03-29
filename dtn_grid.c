/* 
 * Iain Workman
 * ipw969
 * 11139430
 */
#include "dtn_grid.h"

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

    int i_step = 0;


    if (!grid) {
        return -1;
    }

    if (step_count <= 0) {
        return -1;
    }

    if (!propagation_function) {
        return -1;
    }

    /* Spin up the threads */
    /* for i = 0 to step_count -1: */

}