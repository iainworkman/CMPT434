/* Created for CMPT332 - Autumn Term 2016				*/
/* Name: Ryan LaForge          Name: Iain Workman		*/
/* NSID: ral362                NSID: ipw969				*/
/* Student No.: 11137909       Student No.: 11139430	*/
/*														*/
#include "list.h"
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

extern NODE* _free_nodes;
extern int _nodes_in_use;
extern int _lists_in_use;
extern int _total_user_lists;
extern int _total_user_nodes;
extern NODE* _nodes_for_free_lists;
extern LIST* _free_lists;
void *ListRemove(LIST* list) {
	void* item;
	NODE* removed_node;
    if (!list || !list->is_in_use || list->count == 0) {
        return NULL;
    }
	removed_node = list->current;
	item = list->current->data;
	/* Take the node out of the list */
	if (list->count == 1) {
		list->first = NULL;
		list->last = NULL;
		list->current = NULL;
	}
	else if (list->first == list->current) {
		list->first = list->current->next;
		list->current->next->previous = NULL;
		list->current = list->current->next;
	}
	else if (list->last == list->current) {
		list->current->previous->next = NULL;
		list->last = list->current->previous;
		list->current = list->current->previous;
	}
	else {
		list->current->previous->next = list->current->next;
		list->current->next->previous = list->current->previous;
		list->current = list->current->next;
	}

	list->count--;
	_nodes_in_use--;
	/* Put the node back into free_nodes */
	removed_node->next = _free_nodes;
	_free_nodes = removed_node;

	/* Clean up the newly freed node */
	removed_node->data = NULL;
	removed_node->previous = NULL;

	return item;
}

void ListFree(LIST* list, void (*itemFree)(void*)) {
	NODE* current_node;
	NODE* node_for_free_list;
    if (!list || !list->is_in_use) {
        return;
    }
	while (list->first != NULL) {
		/* Remove the node from the list */
		current_node = list->first;
		current_node->previous = NULL;
		list->first = current_node->next;
		
		/* Free the item with user-provided func */
		if (itemFree != NULL) {
			(*itemFree)(current_node->data);
		}
		current_node->data = NULL;

		list->count--;
		_nodes_in_use--;

		/* Put the node back into free_nodes */
		current_node->next = _free_nodes;
		_free_nodes = current_node;

		/* Clean up the newly freed node */
		current_node->data = NULL;
		current_node->previous = NULL;
	}

	list->last = NULL;
	list->current = NULL;
	list->is_in_use = 0;
	/* Remove a node from the list of 
	 * _node_for_free_lists to store nodes */
	node_for_free_list = _nodes_for_free_lists;
	_nodes_for_free_lists = _nodes_for_free_lists->next;
	node_for_free_list->next = NULL;

	/* Put the list head into that node, and put that node in 
	 * _free_lists */
	node_for_free_list->data = list;
	node_for_free_list->previous = _free_lists->last;
	_free_lists->last = node_for_free_list;
	_free_lists->count++;

	_lists_in_use--;
}

void *ListTrim(LIST* list) {
	void* item;
	NODE* removed_node;
    if (!list || !list->is_in_use || list->count == 0) {
        return NULL;
    }
	removed_node = list->last;
	item = removed_node->data;	

	if (list->count == 1) {
		list->first = NULL;
		list->last = NULL;
	}
	else if (list->count > 1) {
		list->last->previous->next = NULL;
		list->last = list->last->previous;
		list->current = list->last;
	}
	list->count--;
	_nodes_in_use--;

	/* Put the node back into free_nodes */
	removed_node->next = _free_nodes;
	_free_nodes = removed_node;

	/* Clean up the newly freed node */
	removed_node->data = NULL;
	removed_node->previous = NULL;

	return item;
}
