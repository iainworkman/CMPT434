/* 
 * Iain Workman
 * ipw969
 * 11139430
 */
#include "policies.h"
#include "dtn.h"

int always_send(dtn_node *self,
                dtn_node *other,
                dtn_message *message,
                int *should_remove) {

    *should_remove = 1;
    return 1;
}
