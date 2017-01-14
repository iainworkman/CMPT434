/*
 * Name: Iain Workman
 * NSID: ipw969
 * Student No.: 11139430
 */

#include "calendar.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {

	CalendarEntry test_entry1, test_entry2, test_entry3, test_entry4;
	CalendarEntry fetch_spec;
	Calendar* calendar = 0;
	if(CalendarInit() != 0) {

		return 1;
	}	

	ParseDate("160114", &test_entry1.date);
	ParseTime("1600", &test_entry1.start_time);
	ParseTime("1630", &test_entry1.end_time);
	strcpy(test_entry1.name, "Middle");

	if(CalendarAdd(&test_entry1, "Iain") != 0) {

		printf("Failed to add entry 1\n");
		return 1;
	}

	ParseDate("160114", &test_entry2.date);
	ParseTime("1640", &test_entry2.start_time);
	ParseTime("1650", &test_entry2.end_time);
	strcpy(test_entry2.name, "Last");

	if(CalendarAdd(&test_entry2, "Iain") != 0) {
		printf("Failed to add entry 2\n");
		return 1;
	}

	ParseDate("150114", &test_entry3.date);
	ParseTime("1500", &test_entry3.start_time);
	ParseTime("1600", &test_entry3.end_time);
	strcpy(test_entry3.name, "First");

	if(CalendarAdd(&test_entry3, "Iain") != 0) {
		printf("Failed to add entry 3\n");
		return 1;
	}

	ParseDate("150114", &test_entry4.date);
	ParseTime("1500", &test_entry4.start_time);
	ParseTime("1600", &test_entry4.end_time);
	strcpy(test_entry4.name, "Sole");

	if(CalendarAdd(&test_entry4, "Other") != 0) {
		printf("Failed to add entry 4\n");
		return 1;
	}
	

	fetch_spec.date.empty = 1;
	fetch_spec.start_time.empty = 1;
	fetch_spec.end_time.empty = 1;

	calendar = CalendarGetEntries(&fetch_spec, "Iain");

	if(!calendar || ListCount(calendar->entries) == 0) {
		printf("Failed to return Iain's calendar\n");
		return 1;
	} else {
		PrintCalendar(calendar);
	}

	calendar = CalendarGetEntries(&fetch_spec, "Other");

	if(!calendar || ListCount(calendar->entries) == 0) {
		printf("Failed to return other's calendar\n");
		return 1;
	} else {
		PrintCalendar(calendar);
	}

	return 0;
}
