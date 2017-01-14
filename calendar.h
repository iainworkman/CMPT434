/*
 * Name: Iain Workman
 * NSID: ipw969
 * Student No.: 11139430
 */

#ifndef _CALENDAR_H_
#define _CALENDAR_H_

#include "list.h"

#define MAX_NAME_LENGTH 255
#define MAX_USERNAME_LENGTH 255

/*
 * A single calendar entry
 */
typedef struct CalendarEntry {
	char date[7];
	char start_time[5];
	char end_time[5];
	char name[MAX_NAME_LENGTH];
	char username[MAX_USERNAME_LENGTH];

} CalendarEntry;

/*
 * Adds an entry based on the provided calendar entry
 * @returns 0 on success, -1 on failure
 */
int CalendarAdd(CalendarEntry entry);

/*
 * Removes an entry based on the provided calendar entry
 * @returns 0 on success, -1 on failure (e.g. if no matching entry found)
 */
int CalendarRemove(CalendarEntry entry);

/*
 * Updates the entry in the calendar which matches the provided entry 
 * to the values specified in new_entry.
 * @returns 0 on success, -1 on failure (e.g. if no matching entry found)
 */
int CalendarUpdate(CalendarEntry entry, CalendarEntry new_entry);

/*
 * Gets a list of entries which match the details provided by entry 
 * in the following manner:
 *   - If a date and start_time are provided will return entries which match
 *     the date and start time.
 *   - If just a date is provided will return all entries which occur on that
 *     date.
 *   - If nothing is provided will return all entries from the user's 
 *     calendar.
 * NOTE: The resources for the returned LIST* are the responsibility of the
 * caller.
 */
LIST* CalendarGetEntries(CalendarEntry entry);

#endif //_CALENDAR_H_
