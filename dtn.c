/* 
 * Iain Workman
 * ipw969
 * 11139430
 */
#include "dtn.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

int message_id;

int already_received(void *message1_ptr, void *message2_ptr) {

    dtn_message *message1 = (dtn_message *) message1_ptr;
    dtn_message *message2 = (dtn_message *) message2_ptr;

    if (message1->id == message2->id) {
        return 1;
    } else {
        return 0;
    }
}

dtn_grid *
init_grid(int node_count, int destination_count, int node_buffer_size) {

    int i_node;
    dtn_grid *new_grid;

    if (node_count <= 0 || destination_count > node_count) {
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

    for (i_node = 0; i_node < node_count; i_node++) {
        dtn_node *current_node = &new_grid->nodes[i_node];

        current_node->id = i_node;
        current_node->x_position = rand() % 1000;
        current_node->y_position = rand() % 1000;
        current_node->messages = ListCreate();
        current_node->buffer_size = node_buffer_size;
        current_node->broadcasts_made = 0;
        current_node->messages_lost = 0;
    }
    return new_grid;
}

int run_simulation(dtn_grid *grid,
                   simulation_settings settings,
                   int (*propagation_function)(dtn_node *, dtn_node *,
                                               dtn_message *, int *),
                   simulation_statistics *statistics) {

    int i_step, i_current, i_other;

    if (!statistics || !grid || !propagation_function) {
        return -1;
    }

    statistics->total_messages_lost = 0;
    statistics->messages_in_flight = 0;
    statistics->total_broadcasts = 0;
    statistics->messages_at_destination = 0;

    message_id = 0;

    for (i_step = 0; i_step < settings.step_count; i_step++) {

        if (verbose_mode) printf("Step %d\n", i_step);
        /* 1. Move Nodes */
        for (i_current = 0; i_current < grid->node_count; i_current++) {
            dtn_node *self = &grid->nodes[i_current];
            if (verbose_mode) printf("\tMoving node %d", i_current);

            node_move(self, settings.move_distance);
            if (verbose_mode)
                printf(" now at (%f, %f)\n", self->x_position,
                       self->y_position);
        }

        /* 2. Generate 1 Message at each node */
        for (i_current = 0; i_current < grid->node_count; i_current++) {
            int destination_id;
            dtn_node *self = &grid->nodes[i_current];
            dtn_message *new_message = malloc(sizeof(dtn_message));
            
            if (verbose_mode) {
                printf("\tGenerating messages at node %d\n", i_current);
            }


            new_message->id = message_id;
            message_id++;
            new_message->source = self;

            /* Get random destination which is not the source */
            do {
                destination_id = rand() % grid->destination_count;
            } while (destination_id == i_current);

            new_message->destination = &grid->nodes[destination_id];
            new_message->hops_taken = 0;

            if (ListCount(self->messages) < self->buffer_size) {

                ListAppend(self->messages, new_message);
            } else {
                /* Buffer is out of space */
                self->messages_lost++;
            }
        }

        /* 3. Broadcasting */
        for (i_current = 0; i_current < grid->node_count; i_current++) {

            dtn_node *self = &grid->nodes[i_current];

            if (verbose_mode) printf("\tSending messages from %d\n", i_current);
            for (i_other = 0; i_other < grid->node_count; i_other++) {
                dtn_node *other = &grid->nodes[i_other];
                if (i_current == i_other) {
                    /* Do not send to self */
                    continue;
                }

                if (node_in_range(self, other, settings.broadcast_range)) {

                    dtn_message *current_message;

                    current_message = ListFirst(self->messages);
                    /* Check all the messages in self to see if should broadcast */
                    while (current_message) {

                        int should_remove = 0;

                        if (current_message->destination == other) {
                            /* Always Attempt Broadcast if node is destination */

                            if (ListSearch(other->messages, already_received,
                                           current_message) == 0) {

                                /* Copy the message */
                                dtn_message *message_copy = malloc(
                                        sizeof(dtn_message));

                                message_copy->id = current_message->id;
                                message_copy->destination = current_message->destination;
                                message_copy->source = current_message->source;
                                message_copy->hops_taken =
                                        current_message->hops_taken + 1;

                                ListAdd(other->messages, message_copy);
                                self->broadcasts_made++;
                            }
                            ListRemove(self->messages);
                            current_message = ListFirst(self->messages);
                            if (verbose_mode) printf("\t\tto %d\n", i_other);
                        } else if (ListCount(other->messages) >=
                                   other->buffer_size) {
                            /* No Room */
                            current_message = ListNext(self->messages);

                        } else if (propagation_function(self,
                                                        other,
                                                        current_message,
                                                        &should_remove) &&
                                   current_message->destination != self) {

                            /* Broadcast if prop func says we should, and the
                             * message isn't already at the destination */

                            if (ListSearch(other->messages, already_received,
                                           current_message) == 0) {
                                /* Copy the message */
                                dtn_message *message_copy = malloc(
                                        sizeof(dtn_message));

                                message_copy->id = current_message->id;
                                message_copy->destination = current_message->destination;
                                message_copy->source = current_message->source;
                                message_copy->hops_taken =
                                        current_message->hops_taken + 1;

                                ListAdd(other->messages, message_copy);
                                self->broadcasts_made++;

                                if (should_remove) {
                                    dtn_message *sent = ListRemove(
                                            self->messages);
                                    free(sent);
                                    current_message = ListFirst(self->messages);
                                } else {
                                    current_message = ListNext(self->messages);
                                }
                                if (verbose_mode)
                                    printf("\t\tto %d\n", i_other);
                            } else {
                                current_message = ListNext(self->messages);
                            }

                        } else {
                            current_message = ListNext(self->messages);
                        }
                    }
                }
            }
        }
    }

    /* Done all steps ~ Let's collect some statistics */
    for (i_current = 0; i_current < grid->node_count; i_current++) {

        dtn_node *self = &grid->nodes[i_current];
        dtn_message *current_message;

        while ((current_message = ListRemove(self->messages)) != 0) {

            if (current_message->destination == self) {
                statistics->messages_at_destination++;
            } else {
                statistics->messages_in_flight++;
            }

            free(current_message);
        }

        statistics->total_broadcasts += self->broadcasts_made;
        statistics->total_messages_lost += self->messages_lost;
    }

    return 0;
}

int node_in_range(dtn_node *node1, dtn_node *node2, float range) {

    float delta_x;
    float delta_y;
    float distance;

    delta_x = node1->x_position - node2->x_position;
    delta_y = node1->y_position - node2->y_position;

    distance = sqrt(pow(delta_x, 2) + pow(delta_y, 2));

    if (distance <= range) {
        return 1;
    } else {
        return 0;
    }
}

int node_move(dtn_node *node, float distance) {
    int angle_degrees;
    int angle_radians;

    if (!node) {
        return 0;
    }

    angle_degrees = rand() % 360;
    angle_radians = angle_degrees * (M_PI / 180);

    /* Check the on the line cases */
    if (angle_degrees == 0) {
        node->x_position = min(node->x_position + distance, MAX_POSITION);
        return 1;
    } else if (angle_degrees == 90) {
        node->y_position = min(node->y_position + distance, MAX_POSITION);
        return 1;
    } else if (angle_degrees == 180) {
        node->x_position = max(node->x_position + distance, MIN_POSITION);
        return 1;
    } else if (angle_degrees == 270) {
        node->y_position = max(node->y_position + distance, MIN_POSITION);
        return 1;
    }

    /* Check the quadrant cases */
    if (angle_degrees > 0 && angle_degrees < 90) {
        float delta_x;
        float delta_y;

        delta_x = sin(angle_radians) * distance;
        delta_y = cos(angle_radians) * distance;

        node->x_position = min(node->x_position + delta_x, MAX_POSITION);
        node->y_position = min(node->y_position + delta_y, MAX_POSITION);
    } else if (angle_degrees > 90 && angle_degrees < 180) {
        float delta_x;
        float delta_y;

        angle_degrees = angle_degrees - 90;

        delta_x = cos(angle_radians) * distance;
        delta_y = sin(angle_radians) * distance;

        node->x_position = max(node->x_position + delta_x, MIN_POSITION);
        node->y_position = min(node->y_position + delta_y, MAX_POSITION);
    } else if (angle_degrees > 180 && angle_degrees < 270) {
        float delta_x;
        float delta_y;

        angle_degrees = angle_degrees - 180;

        delta_x = sin(angle_radians) * distance;
        delta_y = cos(angle_radians) * distance;

        node->x_position = max(node->x_position + delta_x, MIN_POSITION);
        node->y_position = max(node->y_position + delta_y, MIN_POSITION);
    } else if (angle_degrees > 270 && angle_degrees < 360) {
        float delta_x;
        float delta_y;

        angle_degrees = angle_degrees - 270;

        delta_x = cos(angle_radians) * distance;
        delta_y = sin(angle_radians) * distance;

        node->x_position = min(node->x_position + delta_x, MAX_POSITION);
        node->y_position = max(node->y_position + delta_y, MIN_POSITION);
    }

    return 1;
}
