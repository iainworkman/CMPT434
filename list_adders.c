/* Created for CMPT332 - Autumn Term 2016				*/
/* Name: Ryan LaForge          Name: Iain Workman		*/
/* NSID: ral362                NSID: ipw969				*/
/* Student No.: 11137909       Student No.: 11139430	*/
/*														*/
#include "list.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

NODE* 	_free_nodes = NULL;
LIST* 	_free_lists = NULL;
NODE*   _nodes_for_free_lists = NULL;
int 	_initialized_data = 0;
int		_lists_in_use = 0;
int 	_nodes_in_use = 0;
int 	_total_user_lists = 0;
int 	_total_user_nodes = 0;

LIST *ListCreate() {

	NODE* node_storing_list = 0;
	LIST* obtained_list = 0;
	if(!_initialized_data) {

		int i_node = 0;
		int i_list = 0;
		_free_nodes = (NODE*)malloc(sizeof(NODE) * MIN_NODES);
		_nodes_for_free_lists = (NODE*)malloc(sizeof(NODE) * MIN_LISTS);
		_free_lists = (LIST*)malloc(sizeof(LIST) * (MIN_LISTS + 1));
		_total_user_nodes = MIN_NODES;
		_total_user_lists = MIN_LISTS;
		if (!_free_nodes || !_nodes_for_free_lists || !_free_lists) {
			return NULL;
		}
		/* Set up the nodes */
		/* Iterate through and chain each one with the next */
		for(i_node = 0; i_node < (MIN_NODES - 1); ++i_node) {
		
			_free_nodes[i_node].next = &(_free_nodes[i_node+1]);
			_free_nodes[i_node].previous = NULL;
			_free_nodes[i_node].data = NULL;
		}
		/* Set the last one's next to NULL */
		_free_nodes[MIN_NODES - 1].next = NULL;
		_free_nodes[MIN_NODES - 1].previous = NULL;
		_free_nodes[MIN_NODES - 1].data = NULL;

		/* Set up the nodes for the free list */
		for(i_node = 0; i_node < (MIN_LISTS - 1); ++i_node) {

			_nodes_for_free_lists[i_node].next = 
					&(_nodes_for_free_lists[i_node+1]);
			_nodes_for_free_lists[i_node].previous = NULL;
			_nodes_for_free_lists[i_node].data = NULL;
		}
		/* Set the last one's next to NULL */
		_nodes_for_free_lists[MIN_LISTS - 1].next = NULL;
		_nodes_for_free_lists[MIN_LISTS - 1].previous = NULL;
		_nodes_for_free_lists[MIN_LISTS - 1].data = NULL;

		/* Set up the lists */
		/* Use the first as the head of the list of free lists */
		_free_lists[0].first = NULL;
		_free_lists[0].last = NULL;
		_free_lists[0].current = NULL;
		_free_lists[0].is_in_use = 1;
		_free_lists[0].count = 0;

		/* iterate through the lists, set them to safe initial values and add
			then to the free list */	
		for(i_list = 1; i_list <= MIN_LISTS; ++i_list) {
			
			NODE* node_for_list = 0;
			_free_lists[i_list].first = NULL;
			_free_lists[i_list].last = NULL;
			_free_lists[i_list].current = NULL;
			_free_lists[i_list].is_in_use = 0;
			_free_lists[i_list].count = 0;

			node_for_list = _nodes_for_free_lists;

			/* Do the node mambo */
			if(!node_for_list) {
				/* We've somehow run out of nodes while building the free list,
					this shouldn't happen, but let's do some output just in 
					case */
				printf("[CRITICAL] Ran out of nodes building the free list\n");
				return 0;
			}
			_nodes_for_free_lists = node_for_list->next;

			node_for_list->data = &(_free_lists[i_list]);
			
			/* Add node to the list */
			if(_free_lists[0].first == NULL ) {
				/* This is the first addition */
				_free_lists[0].first = node_for_list;
				_free_lists[0].last = node_for_list;
				_free_lists[0].current = node_for_list;
				_free_lists[0].count = _free_lists[0].count + 1;
			} else {
				_free_lists[0].last->next = node_for_list;
				node_for_list->previous = _free_lists[0].last;
				_free_lists[0].last = node_for_list;			
				_free_lists[0].count = _free_lists[0].count + 1;

			}
		} 

		_initialized_data = 1;
	}

	if (_free_lists->count == 0) {
		
		int i_node = 0;
		int i_list = 0;
		NODE* more_nodes_for_lists = 
				(NODE*)malloc(sizeof(NODE) * _lists_in_use);

		/* Grab more memory for lists, add it to the free list  */
		LIST* more_free_lists = (LIST*)malloc(sizeof(LIST) * _lists_in_use);
		_total_user_lists += _lists_in_use;
		if (!more_free_lists || !more_nodes_for_lists) {
			return NULL;
		}
		for (i_node = 0; i_node < _lists_in_use-1; ++i_node) {
			more_nodes_for_lists[i_node].next = 
					&(more_nodes_for_lists[i_node + 1]);

			if(i_node == 0) {
				more_nodes_for_lists[i_node].previous = NULL;
			} else {
				more_nodes_for_lists[i_node].previous = 
						&(more_nodes_for_lists[i_node - 1]);
			}
			more_nodes_for_lists[i_node].data = NULL;
		}

		more_nodes_for_lists[_lists_in_use - 1].next = NULL;
		more_nodes_for_lists[_lists_in_use - 1].previous = 
					&(more_nodes_for_lists[_lists_in_use - 2]);
		more_nodes_for_lists[_lists_in_use - 1].data = NULL;	

		for(i_list = 0; i_list < _lists_in_use; ++i_list) {
			more_nodes_for_lists[i_list].data = &(more_free_lists[i_list]);	
		}

		_free_lists->first = &(more_nodes_for_lists[0]);
		_free_lists->last = &(more_nodes_for_lists[_lists_in_use - 1]);
		_free_lists->count += _lists_in_use;
	}

	/* Get node containing list from the free list */
	node_storing_list = _free_lists->last;
	
	if(node_storing_list->previous) {
		node_storing_list->previous->next = NULL;
	} else {
		/* We've just removed the last node */
		_free_lists->first = NULL;
		_free_lists->current = NULL;
	}

	_free_lists->last = node_storing_list->previous;
	_free_lists->count = _free_lists->count - 1;
	
	obtained_list = (LIST*)(node_storing_list->data);

	/* Put the node back in the free node 'list' */

	node_storing_list->data = NULL;
	node_storing_list->previous = NULL;
	node_storing_list->next = _nodes_for_free_lists;
	_nodes_for_free_lists = node_storing_list;

	obtained_list->first = NULL;
	obtained_list->last = NULL;
	obtained_list->current = NULL;
	obtained_list->count = 0;
	obtained_list->is_in_use = 1;

	_lists_in_use++;
	return obtained_list;	
}

int ListAdd(LIST* list, void* item) {
    NODE* node_to_add = 0;
		
	if (!list || !list->is_in_use) {
        return -1;
    }

    if (!item) {
        return -1;
    }
	
	/* Get a NODE */
	if(!_free_nodes) {
		int i_node = 0;
		/* We've run out of free nodes */
		NODE* more_free_nodes = (NODE*)malloc(sizeof(NODE) * _nodes_in_use);
		_total_user_nodes += _nodes_in_use;
		if (!more_free_nodes) {
			return -1;
		}
		for(i_node = 0; i_node < _nodes_in_use - 1; ++i_node) {
			more_free_nodes[i_node].next = &(more_free_nodes[i_node + 1]);
			more_free_nodes[i_node].data = NULL;
			more_free_nodes[i_node].previous = NULL;
		}

		more_free_nodes[_nodes_in_use - 1].next = NULL;
		more_free_nodes[_nodes_in_use - 1].previous = NULL;
		more_free_nodes[_nodes_in_use - 1].data = NULL;
		
		_free_nodes = more_free_nodes;
	}

	node_to_add = _free_nodes;
	_free_nodes = node_to_add->next;

	/* Set valid init data for the node */
	node_to_add->next = NULL;
	node_to_add->previous = NULL;
	node_to_add->data = item;

	/* Add the NODE to the LIST */
	if(!list->first) {
		/* This is the first node */
		list->first = node_to_add;
		list->last = node_to_add;
		list->current = node_to_add;
	} else if (list->current == list->last) {
		/* We're adding after the current last element */
		list->last->next = node_to_add;
		node_to_add->previous = list->last;
		list->last = node_to_add;
		list->current = node_to_add;
	} else {
		/* We're in the middle somewhere */
		node_to_add->previous = list->current;
		node_to_add->next = list->current->next;
		list->current->next = node_to_add;
		node_to_add->next->previous = node_to_add;
		list->current = node_to_add;
	}

	_nodes_in_use += 1;
	list->count += 1;
    return 0;
}

int ListInsert(LIST* list, void* item) {
	NODE* node_to_add = 0;
		
	if (!list || !list->is_in_use) {
        return -1;
    }

    if (!item) {
        return -1;
    }
	
	/* Get a NODE */
	if(!_free_nodes) {
		int i_node = 0;
		/* We've run out of free nodes */
		NODE* more_free_nodes = (NODE*)malloc(sizeof(NODE) * _nodes_in_use);
		_total_user_nodes += _nodes_in_use;
		if (!more_free_nodes) {
			return -1;
		}
		for(i_node = 0; i_node < _nodes_in_use - 1; ++i_node) {
			more_free_nodes[i_node].next = &(more_free_nodes[i_node + 1]);
			more_free_nodes[i_node].data = NULL;
			more_free_nodes[i_node].previous = NULL;
		}

		more_free_nodes[_nodes_in_use - 1].next = NULL;
		more_free_nodes[_nodes_in_use - 1].previous = NULL;
		more_free_nodes[_nodes_in_use - 1].data = NULL;
		
		_free_nodes = more_free_nodes;
	}

	node_to_add = _free_nodes;
	_free_nodes = node_to_add->next;


	/* Set valid init data for the node */
	node_to_add->next = NULL;
	node_to_add->previous = NULL;
	node_to_add->data = item;

	/* Add the NODE to the LIST */
	if(!list->first) {
		/* This is the first node */
		list->first = node_to_add;
		list->last = node_to_add;
		list->current = node_to_add;
	} else if (list->current == list->first) {
		/* We're adding before the current first element */
		list->first->previous = node_to_add;
		node_to_add->next = list->first;
		list->first = node_to_add;
		list->current = node_to_add;
	} else {
		/* We're in the middle somewhere */
		node_to_add->next = list->current;
		node_to_add->previous = list->current->previous;
		list->current->previous = node_to_add;
		node_to_add->previous->next = node_to_add;
		list->current = node_to_add;
	}

	_nodes_in_use += 1;
	list->count += 1;
    return 0;
}

int ListAppend(LIST* list, void* item) {
	NODE* node_to_add = 0;
		
	if (!list || !list->is_in_use) {
        return -1;
    }

    if (!item) {
        return -1;
    }
	
	/* Get a NODE */
	if(!_free_nodes) {
		int i_node = 0;
		/* We've run out of free nodes */
		NODE* more_free_nodes = (NODE*)malloc(sizeof(NODE) * _nodes_in_use);
		_total_user_nodes += _nodes_in_use;
		if (!more_free_nodes) {
			return -1;
		}
		for(i_node = 0; i_node < _nodes_in_use - 1; ++i_node) {
			more_free_nodes[i_node].next = &(more_free_nodes[i_node + 1]);
			more_free_nodes[i_node].data = NULL;
			more_free_nodes[i_node].previous = NULL;
		}

		more_free_nodes[_nodes_in_use - 1].next = NULL;
		more_free_nodes[_nodes_in_use - 1].previous = NULL;
		more_free_nodes[_nodes_in_use - 1].data = NULL;
		
		_free_nodes = more_free_nodes;
	}

	node_to_add = _free_nodes;
	_free_nodes = node_to_add->next;

	/* Set valid init data for the node */
	node_to_add->next = NULL;
	node_to_add->previous = NULL;
	node_to_add->data = item;

	/* Add the NODE to the LIST */
	if(!list->last) {
		/* This is the first node */
		list->first = node_to_add;
		list->last = node_to_add;
		list->current = node_to_add;
	} else  {
		list->last->next = node_to_add;
		node_to_add->previous = list->last;
		list->last = node_to_add;
		list->current = node_to_add;
	}

	_nodes_in_use += 1;
	list->count += 1;
    return 0;
}

int ListPrepend(LIST* list, void* item) {
	NODE* node_to_add = 0;
		
	if (!list || !list->is_in_use) {
        return -1;
    }

    if (!item) {
        return -1;
    }
	
	/* Get a NODE */
	if(!_free_nodes) {
		int i_node = 0;
		/* We've run out of free nodes */
		NODE* more_free_nodes = (NODE*)malloc(sizeof(NODE) * _nodes_in_use);
		_total_user_nodes += _nodes_in_use;
		if (!more_free_nodes) {
			return -1;
		}
		for(i_node = 0; i_node < _nodes_in_use - 1; ++i_node) {
			more_free_nodes[i_node].next = &(more_free_nodes[i_node + 1]);
			more_free_nodes[i_node].data = NULL;
			more_free_nodes[i_node].previous = NULL;
		}

		more_free_nodes[_nodes_in_use - 1].next = NULL;
		more_free_nodes[_nodes_in_use - 1].previous = NULL;
		more_free_nodes[_nodes_in_use - 1].data = NULL;
		
		_free_nodes = more_free_nodes;
	}

	node_to_add = _free_nodes;
	_free_nodes = node_to_add->next;

	/* Set valid init data for the node */
	node_to_add->next = NULL;
	node_to_add->previous = NULL;
	node_to_add->data = item;

	/* Add the NODE to therLIST */
	if(!list->first) {
		/* This is the first node */
		list->first = node_to_add;
		list->last = node_to_add;
		list->current = node_to_add;
	} else  {
		list->first->previous = node_to_add;
		node_to_add->next = list->first;
		list->first = node_to_add;
		list->current = node_to_add;
	}

	_nodes_in_use += 1;
	list->count += 1;
    return 0;
}

void ListConcat(LIST* list1, LIST* list2) {
	NODE* free_list_node = 0;	

	if (!list1 || !list1->is_in_use) {
        return;
    }

	if(!list2 || !list2->is_in_use) {
		return;
	}

	if(list1 == list2) {
		return;
	}
	
	if(list1->count == 0) {
		list1->first = list2->first;
		list1->last = list2->last;
		list1->current = list1->first;
	}
	
	else if (list2->count != 0) {
		list1->last->next = list2->first;
		list1->last = list2->last;
		list2->first->previous = list1->last;
	}
	
	list1->count += list2->count;
	
	/* Deallocate feilds of list2 */
	list2->first = NULL;
	list2->last = NULL;
	list2->count = 0;
	list2->is_in_use = 0;
	list2->current = NULL;
	
	/* pop a node off of nodes_for_free_list */
	if (!_nodes_for_free_lists) {
		printf("nodes_for_free_list is none in concatLists. "
						"Should never happen");
		return;
	}
	free_list_node = _nodes_for_free_lists;
	_nodes_for_free_lists = _nodes_for_free_lists->next;
	
	free_list_node->data = list2;

	/* append to free_lists */
	free_list_node->previous = _free_lists->last;
	_free_lists->last->next = free_list_node;
	_free_lists->last = free_list_node;
	_free_lists->count++;

	_lists_in_use--;
}
