/*
 * Name: Iain Workman
 * NSID: ipw969
 * Student No.: 11139430
 */

#include "calendar.h"
#include <stdio.h>

int main(int argc, char** argv) {

	int rv = 0;
	Time time;

	rv = ParseTime("1002", &time);

	if(rv == -1) {
		printf("Could not parse time\n");
	} else {

		printf("%d %d\n", time.hour, time.minute);
	}
	

	return 0;
}
