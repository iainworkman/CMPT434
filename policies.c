/* 
 * Iain Workman
 * ipw969
 * 11139430
 */
#include <stdlib.h>

#include "policies.h"
#include "dtn.h"

int endemic(dtn_node *self,
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

    *should_remove = 1;
    if (rand() % 100 < 70) {
        return 1;
    }

    return 0;
}
