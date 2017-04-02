/* Group 07 */
/* Name: Ryan LaForge          Name: Iain Workman	*/
/* NSID: ral362                NSID: ipw969		*/
/* Student No.: 11137909       Student No.: 11139430	*/
/*							*/
#include "list.h"
#include <stdio.h>

int ListCount(LIST *list) {
    if (!list || !list->is_in_use) {
        return 0;
    }

    return list->count;
}

void *ListFirst(LIST *list) {
    if (!list || !list->is_in_use || list->count == 0) {
        return NULL;
    }
    list->current = list->first;
    return list->first->data;
}

void *ListLast(LIST *list) {
    if (!list || !list->is_in_use || list->count == 0) {
        return NULL;
    }
    list->current = list->last;
    return list->last->data;

}

void *ListNext(LIST *list) {
    if (!list || !list->is_in_use || list->count == 0) {
        return NULL;
    }
    if (list->current->next == NULL) {
        return NULL;
    }
    list->current = list->current->next;
    return list->current->data;
}

void *ListPrev(LIST *list) {
    if (!list || !list->is_in_use || list->count == 0) {
        return NULL;
    }
    if (list->current->previous == NULL) {
        return NULL;
    }
    list->current = list->current->previous;
    return list->current->data;
}

void *ListCurr(LIST *list) {
    if (!list || !list->is_in_use || list->count == 0) {
        return NULL;
    }
    return list->current->data;
}

void *
ListSearch(LIST *list, int(*comparator)(void *, void *), void *comparisonArg) {
    if (!list || !list->is_in_use || list->count == 0) {
        return NULL;
    }

    if (!comparator) {
        return NULL;
    }

    while (list->current != NULL) {
        if ((*comparator)(list->current->data, comparisonArg)) {
            return list->current->data;
        }
        list->current = list->current->next;
    }
    list->current = list->last;
    return NULL;
}

