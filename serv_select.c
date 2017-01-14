/*
 * Name: Iain Workman
 * NSID: ipw969
 * Student No.: 11139430
 */

#include "calendar.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {

	CalendarEntry test_entry;
	CalendarEntry fetch_spec;
	Calendar* calendar = 0;
	if(CalendarInit() != 0) {

		return 1;
	}	

	ParseDate("160114", &test_entry.date);
	ParseTime("1600", &test_entry.start_time);
	ParseTime("1630", &test_entry.end_time);
	strcpy(test_entry.name, "Appointment");

	if(CalendarAdd(&test_entry, "Iain") != 0) {

		printf("Failed to add entry\n");
		return 1;
	}

	fetch_spec.date.empty = 1;
	fetch_spec.start_time.empty = 1;
	fetch_spec.end_time.empty = 1;

	calendar = CalendarGetEntries(&fetch_spec, "Iain");

	if(!calendar || ListCount(calendar->entries) == 0) {
		printf("Failed to return calendar\n");
		return 1;
	}

	return 0;
}
