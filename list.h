/* Group 07 */
/* Name: Ryan LaForge          Name: Iain Workman	*/
/* NSID: ral362                NSID: ipw969		*/
/* Student No.: 11137909       Student No.: 11139430	*/
/*							*/
#ifndef _T_LIST_
#define _T_LIST_
#define MIN_NODES 10000
#define MIN_LISTS 1000

typedef struct NODE NODE;
typedef struct LIST LIST;

/*
The data type definition for a node of a list.
*/
struct NODE {
    void *data;
    NODE *next;
    NODE *previous;
};


/*
The data type definition for a linked list.
*/
struct LIST {
    NODE *first;
    NODE *last;
    NODE *current;
    int count;
    int is_in_use;
};


/*
Creates and returns a new, empty list.
@returns A new empty list on success, NULL otherwise.
*/
LIST *ListCreate();

/*
The number of items currently in the list.
@param list: The list whose current number of items is to be returned.
@preconditions: list cannot be NULL. 
		list.is_in_use must be 1.
@returns: The number of items currently stored in the list.
*/
int ListCount(LIST *list);

/*
Moves the internal cursor to the first item in the list and return it.
If the list is empty then no action is taken and NULL is returned.
@param list: The list whose first item is to be selected and returned.
@preconditions: list cannot be NULL. 
		list.is_in_use must be 1.
@returns: The first item in the list, if it exists or NULL if the list
is empty
*/
void *ListFirst(LIST *list);

/*
Moves the internal cursor to the last item in the list and return it.
If the list is empty then no action is taken and NULL is returned.
@param list: The list whose last item is to be selected and returned.
@preconditions: list cannot be NULL. 
		list.is_in_use must be 1.
@returns: The last item in the list, if it exists or NULL if the list
is empty
*/
void *ListLast(LIST *list);

/*
Advances the internal cursor by one, and returns a pointer to the new
current item. If the operation attempts to advance the current item beyond
the end of the list a NULL pointer is returned and the cursor is not moved.
@param list: The list whose internal cursor is to be advanced.
@preconditions: list cannot be NULL. 
		list.is_in_use must be 1.
@returns: The list's new current item, or NULL if there is no item to advance
to.
*/
void *ListNext(LIST *list);

/*
Moves the internal cursor back one, and returns a pointer to the new
current item. If the operation attempts to move the current item beyond
the start of the list a NULL pointer is returned.
@param list: The list whose internal cursor is to be moved back.
@preconditions: list cannot be NULL. 
		list.is_in_use must be 1.
@returns: The list's new current item, or NULL if there is no item to move back
to.
*/
void *ListPrev(LIST *list);

/*
The current item selected by the list's internal cursor.
@param list: The list whose current selected item is to be returned
@preconditions: list cannot be NULL. 
		list.is_in_use must be 1.
@returns: The list's current selected item or NULL if no item is selected
within the list.
*/
void *ListCurr(LIST *list);

/*
Adds the provided item to the list directly after the current selected item,
and makes it the new current item. If the list is empty the item is added at
the beginning of the list.
@param list: The list to which the item is to be added.
@param item: The item which is to be added to the list.
@preconditions: list cannot be NULL. 
		list.is_in_use must be 1.
		item cannot be NULL.
@returns: 0 on successful addition of the item, -1 on failure.
*/
int ListAdd(LIST *list, void *item);

/*
Adds the provided item to the list directly before the current selected item,
and makes it the new current item. If the list is empty the item is added at
the beginning of the list.
@param list: The list to which the item is to be added.
@param item: The item which is to be added to the list.
@preconditions: list cannot be NULL. 
		list.is_in_use must be 1.
		item cannot be NULL.: 
@returns: 0 on successful addition of the item, -1 on failure.
*/
int ListInsert(LIST *list, void *item);

/*
Adds the provided item to the end of the list, and makes it the current
selected item.
@param list: The list to which the item is to be added.
@param item: The item which is to be added to the list.
@preconditions: list cannot be NULL. 
		list.is_in_use must be 1.
		item cannot be NULL.
@returns 0 on successful addition of the item, -1 on failure.
*/
int ListAppend(LIST *list, void *item);

/*
Adds the provided item to the front of the list, and makes it the current
selected item.
@param list: The list to which the item is to be added.
@param item: The item which is to be added to the list.
@preconditions: list cannot be NULL. 
		list.is_in_use must be 1.
		item cannot be NULL.
@returns 0 on successful addition of the item, -1 on failure.
*/
int ListPrepend(LIST *list, void *item);

/*
Removes the current selected item from the list and returns it. Makes the
next item the new current selected item unless the last item in the list
was selected, in which case it makes the previous item the new current 
selected item. If the last item has been removed from the list no item
will be selected.
@param list: The list from which the last item is to be removed and 
returned.
@preconditions: list cannot be NULL. 
		list.is_in_use must be 1.
@returns The item removed from the list if one was removed, NULL otherwise.
*/
void *ListRemove(LIST *list);

/*
Appends the items from list2 onto the end of list1, then destroys list2.
The selected item in list1 remains selected.
@param list1: The list to which the items are to be appended.
@param list2: The list whose items are to be appended to list1, and then
destroyed.
@preconditions: list1 cannot be NULL. 
		list1.is_in_use must be 1.
		list2 cannot be NULL. 
		list2.is_in_use must be 1.
*/
void ListConcat(LIST *list1, LIST *list2);

/*
Deletes the list and frees each of the items it contains using the provided
itemFree routine. If NULL is passed for itemFree no action will be taken to
free the items of the list.
@param list: The list which is to be destroyed.
@param itemFree: The routine which is to be called upon the items within the
list to free their items.
@preconditions: list cannot be NULL. 
		list.is_in_use must be 1.
*/
void ListFree(LIST *list, void (*itemFree)(void *));

/*
Removes the last from the list and returns it. Makes the new last item the
current selected item. If the list is emptied by the operation, then no
item will be selected.
@param list: The list whose last item is to be removed and returned.
@preconditions: list cannot be NULL. 
		list.is_in_use must be 1.
@returns The item removed from the list if one was removed, NULL otherwise.
*/
void *ListTrim(LIST *list);

/*
Searches the list, starting at the current selected item until either the
end is reached or a match is found. A match is found by calling the comparator
with each item and the comparisonArg. If no match is found the last item is
left as the selected item and NULL is returned.
@param list: The list which will be searched
@param comparator: A comparison routine which will be called with each item
being checked and the comparisonArg. If the routine returns 1 a match is
considered to have been found.
@param comparisonArg: The item against which the comparator is checking each
item for a match.
@preconditions: list cannot be NULL. 
		list.is_in_use must be 1.
		comparator cannot be NULL
@returns: The matched item if one is found, NULL otherwise.
*/
void *
ListSearch(LIST *list, int (*comparator)(void *, void *), void *comparisonArg);

#endif
