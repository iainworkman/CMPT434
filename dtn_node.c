/* 
 * Iain Workman
 * ipw969
 * 11139430
 */
#include "dtn_node.h"
#include <time.h>
#include <stdlib.h>
#include <math.h>

int node_init(dtn_node *node, int buffer_size) {
    if (buffer_size < 1) {
        return -1;
    }

    if (!node) {
        return -1;
    }

    node->message_buffer = malloc(sizeof(dtn_message) * buffer_size);

    if (!node->message_buffer) {
        return -1;
    }

    node->x_position = 0;
    node->y_position = 0;
    node->next_sequence_number = 0;
    node->transmission_count = 0;
    node->next_buffer_slot = 0;

    return 0;
}

void node_free(dtn_node* node) {

    free(node->message_buffer);
    free(node);
}

int node_in_range(dtn_node* node1, dtn_node* node2, float range) {

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

int node_move(dtn_node* node, float distance) {
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
        node->x_position = max(node->x_position - distance, MIN_POSITION);
        return 1;
    } else if (angle_degrees == 270) {
        node->y_position = max(node->y_position - distance, MIN_POSITION);
        return 1;
    }

    /* Check the quadrant cases */
    if (angle_degrees > 0 && angle_degrees < 90) {
        float delta_x;
        float delta_y;

        delta_x = sin(angle_radians) * distance;
        delta_y = cos(angle_radians) * distance;

        node->x_position = min(node->x_position + distance, MAX_POSITION);
        node->y_position = min(node->y_position + distance, MAX_POSITION);
    } else if (angle_degrees > 90 && angle_degrees < 180) {
        float delta_x;
        float delta_y;

        angle_degrees = angle_degrees - 90;

        delta_x = cos(angle_radians) * distance;
        delta_y = sin(angle_radians) * distance;

        node->x_position = max(node->x_position - distance, MIN_POSITION);
        node->y_position = min(node->y_position + distance, MAX_POSITION);
    } else if (angle_degrees > 180 && angle_degrees < 270) {
        float delta_x;
        float delta_y;

        angle_degrees = angle_degrees - 180;

        delta_x = sin(angle_radians) * distance;
        delta_y = cos(angle_radians) * distance;

        node->x_position = max(node->x_position - distance, MIN_POSITION);
        node->y_position = max(node->y_position - distance, MIN_POSITION);
    } else if (angle_degrees > 270 && angle_degrees < 360) {
        float delta_x;
        float delta_y;

        angle_degrees = angle_degrees - 270;

        delta_x = cos(angle_radians) * distance;
        delta_y = sin(angle_radians) * distance;

        node->x_position = min(node->x_position + distance, MAX_POSITION);
        node->y_position = max(node->y_position - distance, MIN_POSITION);
    }

    return 1;
}

dtn_message* message_init(dtn_node* source,
                          dtn_node* destination,
                          int sequence_number) {

    if (sequence_number < 0 || !source || !destination) {
        return 0;
    }

    dtn_message* new_message = malloc(sizeof(dtn_message));

    if (!new_message) {
        return 0;
    }

    new_message->sequence_number = sequence_number;
    new_message->source = source;
    new_message->destination = destination;

    return new_message;
}

void message_free(dtn_message* message) {
    free(message);
}