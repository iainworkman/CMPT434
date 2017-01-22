####################################################
# Name: Iain Workman
# NSID: ipw969
# Student No.: 11139430
####################################################

CC=gcc
CFLAGS=-Wall -pedantic -g

UNAME_M=$(shell uname -m)

ARCH=_$(UNAME_M)

all: serv_select serv_proc serv_thread client
tests: list_test

####################
# Tests
####################

list_test: list_test$(ARCH).o libCalendar$(ARCH).a libList$(ARCH).a
	$(CC) $(CFLAGS) -I. -L. -o $@ $^ -lList$(ARCH) -lCalendar$(ARCH)

list_test$(ARCH).o: list_test.c
	$(CC) $(CFLAGS) -c list_test.c -o $@
####################
# Proc Server
####################

serv_proc: serv_proc$(ARCH).o libCalendar$(ARCH).a libList$(ARCH).a \
					 calendar_proc
	$(CC) $(CFLAGS) -I. -L. -o $@ \
				serv_proc$(ARCH).o libCalendar$(ARCH).a libList$(ARCH).a \
				-lList$(ARCH) -lCalendar$(ARCH)

serv_proc$(ARCH).o: serv_proc.c
	$(CC) $(CFLAGS) -c serv_proc.c -o $@

calendar_proc: calendar_proc$(ARCH).o libCalendar$(ARCH).a libList$(ARCH).a
	$(CC) $(CFLAGS) -I. -L. -o $@ $^ -lList$(ARCH) -lCalendar$(ARCH)

calendar_proc$(ARCH).o: calendar_proc.c
	$(CC) $(CFLAGS) -c calendar_proc.c -o $@
####################
# Threaded Server
####################

serv_thread: serv_thread$(ARCH).o libCalendar$(ARCH).a libList$(ARCH).a
	$(CC) $(CFLAGS) -I. -L. -o $@ $^ -lList$(ARCH) -lCalendar$(ARCH)

serv_thread$(ARCH).o: serv_thread.c
	$(CC) $(CFLAGS) -c serv_thread.c -o $@

####################
# Select Server
####################

serv_select: serv_select$(ARCH).o libCalendar$(ARCH).a libList$(ARCH).a 
	$(CC) $(CFLAGS)	-I. -L. -o $@ $^ -lList$(ARCH) -lCalendar$(ARCH)

serv_select$(ARCH).o: serv_select.c
	$(CC) $(CFLAGS) -c serv_select.c -o $@

####################
# Client
####################

client: client$(ARCH).o libCalendar$(ARCH).a libList$(ARCH).a
	$(CC) $(CFLAGS) -I. -L. -o $@ $^ -lList$(ARCH) -lCalendar$(ARCH)

client$(ARCH).o: client.c
	$(CC) $(CFLAGS) -c client.c -o $@

####################
# Calendar
####################

libCalendar$(ARCH).a: calendar$(ARCH).o
	ar rcs $@ $^

calendar$(ARCH).o: calendar.c
	$(CC) $(CFLAGS) -c calendar.c -o $@

####################
# List
####################

libList$(ARCH).a: 	list_adders$(ARCH).o \
										list_movers$(ARCH).o \
									 	list_removers$(ARCH).o
	ar rcs $@ $^ 

list_adders$(ARCH).o: list_adders.c
	$(CC) $(CFLAGS) -c list_adders.c -o $@

list_movers$(ARCH).o: list_movers.c
	$(CC) $(CFLAGS) -c list_movers.c -o $@

list_removers$(ARCH).o: list_removers.c
	$(CC) $(CFLAGS) -c list_removers.c -o $@

.PHONY: clean

clean:
	rm -rf *.o *.a

