/*
 * Name: Iain Workman
 * NSID: ipw969
 * Student No.: 11139430
 */

#include "calendar.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* Data */
LIST* calendars = 0;

/* Private Functions */
int EntryComparator(void* first_entry, void* second_entry) {

	CalendarEntry* first;
	CalendarEntry* second;

	if(!first_entry || !second_entry) {
		return 0;
	}

	first = (CalendarEntry*)first_entry;
	second = (CalendarEntry*)second_entry;

	if(second->date.empty == 1) {
		/* Matches all */
		return 1;
	} else if(second->start_time.empty == 1) {
		/* Only check the date */
		if(first->date.year == second->date.year &&
				first->date.month == second->date.month &&
				first->date.day == second->date.day) {
		
			return 1;
		}
	} else {
		/* Check the date and start time */
		if(first->date.year == second->date.year &&
				first->date.month == second->date.month &&
				first->date.day == second->date.day &&
				first->start_time.hour == second->start_time.hour &&
				first->start_time.minute == second->start_time.minute) {
		
			return 1;
		}
		
	}
	
	return 0;
			
}

/*
 * Finds the calendar belonging to the specified user. If none is found
 * one is created.
 * @returns A calendar belonging to the specified user, or NULL if one
 * could not be initialized.
 */
Calendar* FindCalendar(char* username) {
	Calendar* current_calendar = 0;
	
	if(!calendars) {
		return 0;
	}

	current_calendar = ListFirst(calendars);

	while(current_calendar) {

		if(strcmp(current_calendar->username, username) == 0) {
			ListFirst(calendars);
			return current_calendar;
		}

		current_calendar = ListNext(calendars);
	}	

	ListFirst(calendars);

	if(!current_calendar) {
		current_calendar = malloc(sizeof(Calendar));
		
		strcpy(current_calendar->username, username);
		current_calendar->entries = ListCreate();

		if(!current_calendar->entries) {
			free(current_calendar);
			return 0;
		}

		if(ListAdd(calendars, current_calendar) != 0) {
			free(current_calendar);
			return 0;
		}
	}

	return current_calendar;
}

/* Public Function */

int CalendarInit() {
	calendars = ListCreate();

	if (!calendars) {
		return -1;
	} else {
		return 0;
	}
}

int CalendarAdd(CalendarEntry* entry, char* username) {
	
	Calendar* calendar = 0;
	CalendarEntry* current = 0;	
  int return_code = 0;

	if(!calendars) {
		return ERR_NOINIT;
	}

	calendar = FindCalendar(username);

	if(!calendar) {
		return ERR_NOCALENDAR;
	}

	/* Ensure we add into the right place */

	current = ListFirst(calendar->entries);

	if(!current) {
		/* Empty: Add at start */
		return ListInsert(calendar->entries, entry);
	}

	while(current) {

		if(CompareEntries(entry, current) < 1) {
			/* new entry is equal to or smaller than current */
			/* Insert here */
			return_code = ListInsert(calendar->entries, entry);
			ListFirst(calendar->entries);
			return return_code;			
		}

		current = ListNext(calendar->entries);
	}
	
	/* Must have got to end - Insert here */
	return_code = ListAdd(calendar->entries, entry);
	ListFirst(calendar->entries);

	return return_code;

}

int CalendarRemove(CalendarEntry* entry, char* username) {
	
	Calendar* calendar = 0;

	if(!calendars) {
		return ERR_NOINIT;
	}

	calendar = FindCalendar(username);

	if(!calendar) {
		return ERR_NOCALENDAR;
	}

	ListFirst(calendar->entries);

	CalendarEntry* to_delete = 
				(CalendarEntry*)ListSearch(calendar->entries, &EntryComparator, entry);

	if(to_delete == 0) {
		ListFirst(calendar->entries);
		return -1;
	}

	ListRemove(calendar->entries);
	ListFirst(calendar->entries);
	
	free(to_delete);

	return 0;		
}

int CalendarUpdate(CalendarEntry* entry, 
									 CalendarEntry* new_entry,
									 char* username) {

	Calendar* calendar = 0;

	if(!calendars) {
		return ERR_NOINIT;
	}

	calendar = FindCalendar(username);

	if(!calendar) {
		return ERR_NOCALENDAR;
	}

	ListFirst(calendar->entries);

	CalendarEntry* to_update =
			(CalendarEntry*)ListSearch(calendar->entries, &EntryComparator, entry);

	if(!to_update) {
		ListFirst(calendar->entries);
		return -1;
	} 

	strcpy(to_update->name, new_entry->name);
	to_update->end_time = new_entry->end_time;

	ListFirst(calendar->entries);
	return 0;
}

Calendar* CalendarGetEntries(CalendarEntry* entry, char* username) {
	
	Calendar* calendar = 0;
	Calendar* return_calendar = 0;

	if(!calendars) {
		return 0;
	}

	calendar = FindCalendar(username);

	if(!calendar) {
		return 0;
	}

	return_calendar = malloc(sizeof(Calendar));
	return_calendar->entries = ListCreate();

	if(!return_calendar->entries) {
		free(return_calendar);
		return 0;
	}

	strcpy(return_calendar->username, username);

	CalendarEntry* matched_entry = 0;
	if(!return_calendar) {
		return return_calendar;
	}

	ListFirst(calendar->entries);

	do{
		matched_entry = ListSearch(calendar->entries, &EntryComparator, entry);

		if(matched_entry) {
			ListInsert(return_calendar->entries, matched_entry);
			if(ListNext(calendar->entries) == 0) {
				/* We're at the end of the list */
				break;
			}
		}
	} while(matched_entry);

	ListFirst(calendar->entries);
	
	return return_calendar;
}

void PrintCalendar(Calendar* calendar) {

}

void PrintEntry(CalendarEntry* entry) {

	if(!entry) {
		return;
	}

	printf("####################\n");

	printf("# Name %s\n", entry->name);
	if(!entry->date.empty) {
		printf("# Date: %d/%d/%d\n", 
						entry->date.year, 
						entry->date.month, 
						entry->date.day);
	}

	if(!entry->start_time.empty) {
		printf("# Start: %d:%d\n",
						entry->start_time.hour,
						entry->start_time.minute);
	}

	if(!entry->end_time.empty) {
		printf("# End: %d:%d\n",
						entry->end_time.hour,
						entry->end_time.minute);
	}

	printf("####################\n");
}

int CompareEntries(CalendarEntry* first, CalendarEntry* second) {
	
	int comparison_value = 0;

	if(!first || !second) {
		return 0;
	}

	comparison_value = first->date.year - second->date.year;
	if(comparison_value != 0) {
		return comparison_value;
	}

	comparison_value = first->date.month - second->date.month;
	if(comparison_value != 0) {
		return comparison_value;
	}

	comparison_value = first->date.day - second->date.day;
	if(comparison_value != 0) {
		return comparison_value;
	}

	comparison_value = first->start_time.hour - second->start_time.hour;
	if(comparison_value != 0) {
		return comparison_value;
	}	

	comparison_value = first->start_time.minute - second->start_time.minute;
	
	return comparison_value;
}

int ParseDate(char* string, Date* date) {
	
	char year_string[7];
	char month_string[5];
	char day_string[3];

	int year;
	int month;
	int day;

	int days_in_month;
	char* temp;

	if(!date) {
		return -1;
	}	

	if(strlen(string) != 6) {
		return -1;
	}

	/* Chop up the string */
	strcpy(year_string, string);
	strcpy(month_string, string + sizeof(char)*2);
	strcpy(day_string, string + sizeof(char)*4);

	year_string[2] = '\0';
	month_string[2] = '\0';

	/* Convert each part to numbers */
	year = strtol(year_string, &temp, 0);
	if(year_string == temp || *temp != '\0') {
		return -1;
	}

	month = strtol(month_string, &temp, 0);
	if(month_string == temp || *temp != '\0') {
		return -1;
	}

	day = strtol(day_string, &temp, 0);
	if(day_string == temp || *temp != '\0') {
		return -1;
	}
	
	/* Validate results as date */

	if(year < 0 || month <= 0 || day <= 0) {
		return -1;
	}

	if(month > 12) {
		return -1;
	}

	switch (month) {

		case 2:
			days_in_month = 29;
			break;
		case 9:
		case 4:
		case 6:
		case 11:
			days_in_month = 30;
			break;
		default:
			days_in_month = 31;
	}

	if(day > days_in_month) {
		return -1;
	}

	/* Write to date */
	date->year = year;
	date->month = month;
	date->day = day;
	date->empty = 0;
	return 0;
}

int ParseTime(char* string, Time* time) {
	char hour_string[7];
	char minute_string[5];

	int hour;
	int minute;

	char* temp;

	if(!time) {
		return -1;
	}	

	if(strlen(string) != 4) {
		return -1;
	}

	/* Chop up the string */
	strcpy(hour_string, string);
	strcpy(minute_string, string + sizeof(char)*2);

	hour_string[2] = '\0';

	/* Convert each part to numbers */
	hour = strtol(hour_string, &temp, 0);
	if(hour_string == temp || *temp != '\0') {
		return -1;
	}

	minute = strtol(minute_string, &temp, 0);
	if(minute_string == temp || *temp != '\0') {
		return -1;
	}

	/* Validate results as time */

	if(minute < 0 || minute > 60) {
		return -1;
	}

	if(hour < 0 || hour > 23) {
		return -1;
  }

	time->hour = hour;
	time->minute = minute;
	time->empty = 0;
	return 0;
}
