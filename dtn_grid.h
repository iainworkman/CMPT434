/* 
 * Iain Workman
 * ipw969
 * 11139430
 */
#ifndef CMPT434_DTN_GRID_H
#define CMPT434_DTN_GRID_H

#include "dtn_node.h"

typedef struct dtn_grid {

    dtn_node *nodes;
    int node_count;
    int destination_count;


} dtn_grid;

dtn_grid *grid_init(int node_count, int destination_count);

void grid_free(dtn_grid *grid);

int grid_run(dtn_grid *grid,
             int step_count,
             int (*propagation_function)(dtn_node *, dtn_node *));


#endif //CMPT434_DTN_GRID_H
