/*
 * Name: Iain Workman
 * NSID: ipw969
 * Student No.: 11139430
 */


#include "calendar.h"

#include <string.h>
#include <stdlib.h>

/* Data */
LIST* calendar = 0;

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
		/* Only check the username */
		if(strcmp(first->username, second->username) == 0) {
			return 1;
		}
	} else if(second->start_time.empty == 1) {
		/* Only check the username and date */
		if(strcmp(first->username, second->username) == 0 &&
				first->date.year == second->date.year &&
				first->date.month == second->date.month &&
				first->date.day == second->date.day) {
		
			return 1;
		}
	} else {
		/* Check username, date and start time */
		if(strcmp(first->username, second->username) == 0 &&
				first->date.year == second->date.year &&
				first->date.month == second->date.month &&
				first->date.day == second->date.day &&
				first->start_time.hour == second->start_time.hour &&
				first->start_time.minute == second->start_time.minute) {
		
			return 1;
		}
		
	}
	
	return 0;
			
}

/* Public Function */

int CalendarInit() {
	calendar = ListCreate();

	if (!calendar) {
		return -1;
	} else {
		return 0;
	}
}

int CalendarAdd(CalendarEntry* entry) {
	
	if(!calendar) {
		return ERR_NOINIT;
	}

	return ListAdd(calendar, entry);	

}

int CalendarRemove(CalendarEntry* entry) {
	ListFirst(calendar);

	CalendarEntry* to_delete = 
				(CalendarEntry*)ListSearch(calendar, &EntryComparator, entry);

	if(to_delete == 0) {
		ListFirst(calendar);
		return -1;
	}

	ListRemove(calendar);
	ListFirst(calendar);
	
	free(to_delete);

	return 0;		
}

int CalendarUpdate(CalendarEntry* entry, CalendarEntry* new_entry) {
	ListFirst(calendar);

	CalendarEntry* to_update =
			(CalendarEntry*)ListSearch(calendar, &EntryComparator, entry);

	if(!to_update) {
		ListFirst(calendar);
		return -1;
	} 

	strcpy(to_update->name, new_entry->name);
	to_update->end_time = new_entry->end_time;

	ListFirst(calendar);
	return 0;
}

LIST* CalendarGetEntries(CalendarEntry* entry) {
	LIST* return_list = ListCreate();
	CalendarEntry* matched_entry = 0;
	if(!return_list) {
		return return_list;
	}

	ListFirst(calendar);

	do{
		matched_entry = ListSearch(calendar, &EntryComparator, entry);

		if(matched_entry) {
			ListInsert(return_list, matched_entry);
		}
	} while(matched_entry);

	ListFirst(calendar);
	return return_list;
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
