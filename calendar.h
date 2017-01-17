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

#define ERR_NOINIT 23445				/* System not properly initialized */
#define ERR_NOCALENDAR 23446		/* Unable to find/create user's calendar */


/* Command Codes */
#define ADD_EVENT 		0
#define REMOVE_EVENT 	1
#define UPDATE_EVENT	2
#define GET_EVENTS		3

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
 * A Calendar belonging to a user with a specific username
 */
typedef struct Calendar {
	LIST* entries;
	char username[MAX_USERNAME_LENGTH];
} Calendar;

/*
 * A single calendar entry
 */
typedef struct CalendarEntry {
	Date date;
	Time start_time;
	Time end_time;
	char name[MAX_NAME_LENGTH];

} CalendarEntry;

/* 
 * A command to be performed on a user's calendar
 */
typedef struct CalendarCommand {

	char username[MAX_USERNAME_LENGTH];
	CalendarEntry event;
	int command_code;
} CalendarCommand;

/*
 * Initializes the calendar system.
 * @returns 0 on success, error code on failure
 */
int CalendarInit();

/*
 * Adds an entry based on the provided calendar entry
 * @returns 0 on success, error code on failure
 */
int CalendarAdd(CalendarEntry* entry, char* username);

/*
 * Removes an entry based on the provided calendar entry
 * @returns 0 on success, error code on failure
 */
int CalendarRemove(CalendarEntry* entry, char* username);

/*
 * Updates the entry in the calendar which matches the provided entry 
 * to the values specified in new_entry.
 * @returns 0 on success, error code on failure
 */
int CalendarUpdate(CalendarEntry* entry, 
									 CalendarEntry* new_entry,
									 char* username);

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
Calendar* CalendarGetEntries(CalendarEntry* entry, char* username);

/*
 * Outputs an entire calendar to stdout
 */
void PrintCalendar(Calendar* calendar);

/*
 * Outputs an entry to stdout
 */
void PrintEntry(CalendarEntry* entry);

/*
 * Compares two entries. 
 * @returns -1 if first is before second
 * 				  0 if first and second occurr on the same day
 *				  1 if first is after second	
 */
int CompareEntries(CalendarEntry* first, CalendarEntry* second);

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

/*
 * Parses an array of strings into a CalendarCommand, placing the
 * results in the passed command struct.
 * @returns 0 on success, -1 otherwise.
 */
int ParseCommand(int argc, char** argv, CalendarCommand* command);

#endif //_CALENDAR_H_
