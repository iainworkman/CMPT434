/* 
 * Iain Workman
 * ipw969
 * 11139430
 */

#include "network.h"
#include <stdio.h>

int main(int argc, char **argv) {

    connection_set *connections;

    connections = init_connection_set(10);

    if (connections) {
        printf("Connections made\n");
    } else {
        printf("Something went wrong\n");
    }
}
