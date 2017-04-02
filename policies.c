/* 
 * Iain Workman
 * ipw969
 * 11139430
 */
#include <stdlib.h>

#include "policies.h"
#include "dtn.h"

int endemic_selective_remove(dtn_node *self,
                             dtn_node *other,
                             dtn_message *message,
                             int *should_remove) {

    if (message->hops_taken > 20) {
        *should_remove = 1;
    } else {
        *should_remove = 0;
    }
    return 1;
}

int endemic_always_remove(dtn_node *self,
                          dtn_node *other,
                          dtn_message *message,
                          int *should_remove) {

    *should_remove = 1;
    return 1;
}

int random_transmit(dtn_node *self,
                    dtn_node *other,
                    dtn_message *message,
                    int *should_remove) {


    if (rand() % 100 < 70) {
        *should_remove = 1;
        return 1;
    }
    *should_remove = 0;
    return 0;
}
