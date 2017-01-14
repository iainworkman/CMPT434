/*
 * Name: Iain Workman
 * NSID: ipw969
 * Student No.: 11139430
 */

#ifndef _CALENDAR_H
#define _CALENDAR_H_

#include "list.h"

#define MAX_NAME_LENGTH 255
#define MAX_USERNAME_LENGTH 255

#define ERR_NOINIT 23445

/* A Date (year, month and day) */
typedef struct Date {
	int year;
	int month;
	int day;
	int empty;
} Date;

/* A Time of Day (24 hr) */
typedef struct Time {
	int hour;
	int minute;
	int empty;
} Time;

/*
 * A single calendar entry
 */
typedef struct CalendarEntry {
	Date date;
	Time start_time;
	Time end_time;
	char name[MAX_NAME_LENGTH];
	char username[MAX_USERNAME_LENGTH];

} CalendarEntry;

/*
 * Initializes the calendar system.
 * @returns 0 on success, error code on failure
 */
int CalendarInit();

/*
 * Adds an entry based on the provided calendar entry
 * @returns 0 on success, error code on failure
 */
int CalendarAdd(CalendarEntry* entry);

/*
 * Removes an entry based on the provided calendar entry
 * @returns 0 on success, error code on failure
 */
int CalendarRemove(CalendarEntry* entry);

/*
 * Updates the entry in the calendar which matches the provided entry 
 * to the values specified in new_entry.
 * @returns 0 on success, error code on failure
 */
int CalendarUpdate(CalendarEntry* entry, CalendarEntry* new_entry);

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
LIST* CalendarGetEntries(CalendarEntry* entry);

/*
 * Parses a string into a date. The resultant date is placed in date.
 * @returns 0 on success, -1 on failure.
 */
int ParseDate(char* string, Date* date);

/*
 * Parses a string into a time. The resultant time is placed in time.
 * @returns 0 on success, -1 on failure.
 */
int ParseTime(char* string, Time* time);

#endif //_CALENDAR_H_
