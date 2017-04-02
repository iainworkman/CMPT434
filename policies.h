/* 
 * Iain Workman
 * ipw969
 * 11139430
 */
#ifndef CMPT434_POLICIES_H
#define CMPT434_POLICIES_H

typedef struct dtn_node dtn_node;
typedef struct dtn_message dtn_message;

int always_send(dtn_node *self,
                dtn_node *other,
                dtn_message *message,
                int *should_remove);

#endif //CMPT434_POLICIES_H
